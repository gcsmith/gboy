// gboy - a portable gameboy emulator
// Copyright (C) 2011  Garrett Smith.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <stdio.h>
#include <assert.h>
#include <GL/glew.h>
#include "cmdline.h"
#include "SDL.h"
#include "SDL_thread.h"
#include "gbx.h"
#include "graphics.h"
#include "sound.h"

#define GBOY_EVENT_SYNC 0
#define GBOY_EVENT_PERF 1

typedef struct gbx_thread {
    gbx_context_t *ctx;         // gbx emulator context
    SDL_Thread *thread;         // handle to this thread
    int running;                // control thread termination
    int limit_speed;            // when set, cpu throttling is enabled
    int enable_sound;           // enable or disable sound playback
    int debugger;               // when set, debug tracing is enabled
    int cycles_per_update;      // cycles to execute between each delay
    float clock_rate;           // keep track of freq (in Hz) for throttling
    float real_period;          // period considering integer cycle counts
    uint32_t fb[GFX_FB_SIZE];   // video framebuffer
    sound_t *snd;
} gbx_thread_t;

typedef struct perf_args {
    gbx_context_t *ctx;         // gbx emulator context
    SDL_TimerID id;             // handle to this timer event
    long last_cycles;           // cycle count on last timer event
} perf_args_t;

typedef struct window_state {
    graphics_t *gfx;            // handle to graphics back-end
    SDL_Window *wnd;            // handle to SDL window
    SDL_GLContext *glctx;       // handle to OpenGL render context
    int width, height, fs;      // dimensions and full-screen enable/disable
    int stretch;                // control stretched or aspect correct display
} window_state_t;

// -----------------------------------------------------------------------------
// Insert a user-defined event into the SDL event queue.
INLINE void push_user_event(int code, void *data1, void *data2)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = data1;
    event.user.data2 = data2;

    // PushEvent is a thread-safe operation, so this is ideal for communicating
    // between the emulator thread and SDL window event thread
    SDL_PushEvent(&event);
}

// -----------------------------------------------------------------------------
// Compute the emulator performance in terms of cycles/second.
uint32_t perf_timer_event(uint32_t interval, void *param)
{
    perf_args_t *pa = (perf_args_t *)param;
    float cps = 1000.0f * (pa->ctx->cycles - pa->last_cycles) / interval;
    pa->last_cycles = pa->ctx->cycles;
    push_user_event(GBOY_EVENT_PERF, (void *)(size_t)cps, NULL);
    return interval;
}

// -----------------------------------------------------------------------------
// Set the desired emulator clock frequency, in Hz.
static void set_gbx_frequency(gbx_thread_t *gt, int hz)
{
    const float update_period_ms = 16.666667f;
    gt->clock_rate = (float)hz;
    gt->cycles_per_update = (int)(update_period_ms * gt->clock_rate / 1000.0f);
    gt->real_period = gt->cycles_per_update * 1000.0f / gt->clock_rate;
}

// -----------------------------------------------------------------------------
// Callback fired each time the emulator enters the vertical blank period.
void ext_video_sync(void *data)
{
    gbx_thread_t *gt = (gbx_thread_t *)data;
    gbx_get_framebuffer(gt->ctx, gt->fb);
    push_user_event(GBOY_EVENT_SYNC, NULL, NULL);

    if (gt->enable_sound)
        sound_render(gt->snd, gt->ctx->frame_cycles);
}

// -----------------------------------------------------------------------------
// Callback fired when the emulator switches between normal/double speed.
void ext_speed_change(void *data, int speed)
{
    if (speed) {
        log_dbg("changing to double speed mode\n");
        set_gbx_frequency((gbx_thread_t *)data, CPU_FREQ_CGB);
    }
    else {
        log_dbg("changing to normal speed mode\n");
        set_gbx_frequency((gbx_thread_t *)data, CPU_FREQ_GMB);
    }
}

// -----------------------------------------------------------------------------
// Callback fired each time the LCD switches between enabled/disabled state.
void ext_lcd_enabled(void *data, int enabled)
{
    if (!enabled) {
        // if LCD disabled, set entire display area to solid white
        memset(((gbx_thread_t *)data)->fb, 0xFF, GFX_FB_SIZE);
        push_user_event(GBOY_EVENT_SYNC, NULL, NULL);
    }
}

// -----------------------------------------------------------------------------
void ext_sound_write(void *data, uint16_t addr, uint8_t value)
{
    gbx_thread_t *gt = (gbx_thread_t *)data;
    if (gt->enable_sound)
        sound_write(gt->snd, gt->ctx->frame_cycles, addr, value);
}

// -----------------------------------------------------------------------------
void ext_sound_read(void *data, uint16_t addr, uint8_t *value)
{
    gbx_thread_t *gt = (gbx_thread_t *)data;
    if (gt->enable_sound)
        *value = sound_read(gt->snd, gt->ctx->frame_cycles, addr);
}

// -----------------------------------------------------------------------------
// Entry-point for emulator thread. Execute cycles and perform CPU throttling.
int gbx_thread_run(void *data)
{
    uint64_t prev, curr;
    float elapsed, delay_period, error_accum = 0.0f;
    gbx_thread_t *gt = (gbx_thread_t *)data;
    gbx_context_t *ctx = gt->ctx;

    // set emulator to power on state (or post-bios state if no bios present)
    gbx_power_on(ctx);
    set_gbx_frequency(gt, CPU_FREQ_GMB);

    // execute instructions until the thread is terminated
    while (gt->running) {
        prev = SDL_GetPerformanceCounter();
        gbx_execute_cycles(ctx, gt->cycles_per_update);
        curr = SDL_GetPerformanceCounter();

        if (gt->limit_speed) {
            elapsed = 1000.0f * (curr - prev) / SDL_GetPerformanceFrequency();
            delay_period = gt->real_period - elapsed;
            if (delay_period <= 0) {
                // delay unnecessary if emulator can't perform at full speed
                error_accum = 0.0f;
                continue;
            }

            // compensate for millisecond precision (XXX: could be better)
            error_accum += (delay_period - (int)delay_period);
            if (error_accum >= 1.0f) {
                delay_period += (int)error_accum;
                error_accum -= (int)error_accum;
            }

            SDL_Delay((int)delay_period);
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Create, initialize, and launch the emulator thread.
gbx_thread_t *gbx_thread_create(gbx_context_t *ctx, cmdargs_t *ca)
{
    // create and initialize the emulator thread structure
    gbx_thread_t *gt = (gbx_thread_t *)calloc(1, sizeof(gbx_thread_t));
    gt->ctx = ctx;
    gt->debugger = ca->debugger;
    gt->running = 1;
    gt->limit_speed = !ca->unlock;
    gt->enable_sound = ca->enable_sound;

    gbx_set_userdata(ctx, gt);
    gbx_set_debugger(ctx, gt->debugger);

    if (ca->serial_path)
        gbx_set_serial_log(ctx, ca->serial_path);

    // initialize sound library
    if (gt->enable_sound) {
        log_info("Initializing APU library...\n");
        gt->snd = sound_init(44100, 4096);
        sound_set_freq(gt->snd, CPU_FREQ_GMB);
    }

    // create and launch the emulator thread
    if (NULL == (gt->thread = SDL_CreateThread(gbx_thread_run, gt))) {
        log_err("failed to create gbx emulator thread\n");
        SAFE_FREE(gt);
        return NULL;
    }

    return gt;
}

// -----------------------------------------------------------------------------
// Terminate and destroy the emulator thread. Blocks until thread completes.
void gbx_thread_destroy(gbx_thread_t *gt)
{
    if (!gt) return;

    // signal thread to terminate and block until it does
    gt->running = 0;
    SDL_WaitThread(gt->thread, NULL);

    if (gt->enable_sound) {
        log_info("Shutting down APU library...\n");
        sound_shutdown(gt->snd);
    }

    SAFE_FREE(gt);
}

// -----------------------------------------------------------------------------
// Initialize SDL, OpenGL, and GLEW. Create and display the main window.
int create_sdl_window(window_state_t *ws, cmdargs_t *ca)
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    GLenum rc;

    assert(NULL != ws);
    assert(NULL != ca);

    ws->fs      = ca->fullscreen;
    ws->width   = ca->width;
    ws->height  = ca->height;
    ws->stretch = ca->stretch;

    log_info("Initializing SDL subsystems...\n");
    if (0 > SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        log_err("SDL initialization failed (%s)\n", SDL_GetError());
        return -1;
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (ca->fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    // create the application window
    ws->wnd = SDL_CreateWindow(gboy_title, SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, ws->width, ws->height, flags);
    if (NULL == ws->wnd) {
        log_err("failed to create SDL window (%s)\n", SDL_GetError());
        return -1;
    }

    // create the opengl rendering context and initialize GLEW
    log_info("Initializing GL context...\n");
    if (NULL == (ws->glctx = SDL_GL_CreateContext(ws->wnd))) {
        log_err("failed to create GL context (%s)\n", SDL_GetError());
        return -1;
    }

    log_info("Initializing GLEW...\n");
    if (GLEW_OK != (rc = glewInit())) {
        log_err("failed to initialize GLEW (%s)\n", glewGetErrorString(rc));
        return -1;
    }

    // enable or disable vertical sync
    SDL_GL_SetSwapInterval(ca->vsync ? 1 : 0);
    return 0;
}

// -----------------------------------------------------------------------------
// XXX: Placeholder code -- should allow for configurable control scheme.
static int keysym_to_joypad(int keysym)
{
    switch (keysym) {
    case SDLK_RIGHT:  return INPUT_RIGHT;
    case SDLK_LEFT:   return INPUT_LEFT;
    case SDLK_UP:     return INPUT_UP;
    case SDLK_DOWN:   return INPUT_DOWN;
    case SDLK_z:      return INPUT_A;
    case SDLK_x:      return INPUT_B;
    case SDLK_RETURN: return INPUT_START;
    case SDLK_SPACE:  return INPUT_SELECT;
    }
    return -1;
}

// -----------------------------------------------------------------------------
int process_window_events(gbx_thread_t *gt, window_state_t *ws)
{
    SDL_Event event;
    long cps, pct;
    int input_index;
    char buffer[64];
    graphics_t *gfx = ws->gfx;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_USEREVENT:
            switch (event.user.code) {
            case GBOY_EVENT_PERF:
                // report frequency and % target speed in window title
                cps = (size_t)event.user.data1;
                pct = (long)(100.0f * cps / 4194304.0f + 0.5f);
                snprintf(buffer, 64, "%s [%ldHz %ld%%]", gboy_title, cps, pct);
                SDL_SetWindowTitle(ws->wnd, buffer);
                break;
            case GBOY_EVENT_SYNC:
                // flip the back buffer on each LCD v-blank event
                graphics_update(gfx, gt->fb);
                break;
            }
            break;
        case SDL_WINDOWEVENT:
            // adjust the opengl viewport if the window is resized
            switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                graphics_resize(gfx, event.window.data1, event.window.data2);
                SDL_GL_SwapWindow(ws->wnd);
                break;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.mod & KMOD_ALT) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    // toggle between windowed and fullscreen modes
                    ws->fs = !ws->fs;
                    SDL_SetWindowFullscreen(ws->wnd, ws->fs);
                }
                else if (event.key.keysym.sym == SDLK_F4) {
                    // terminate the application when ALT+F4 pressed
                    return 0;
                }
                else if (event.key.keysym.sym == SDLK_d) {
                    // toggle the debugger
                    gt->debugger = !gt->debugger;
                    gbx_set_debugger(gt->ctx, gt->debugger);
                }
                else if (event.key.keysym.sym == SDLK_u) {
                    // toggle speed throttling
                    gt->limit_speed = !gt->limit_speed;
                    log_info("emulator speed throttling %s\n",
                             gt->limit_speed ? "enabled" : "disabled");
                }

                break;
            }

            if (-1 != (input_index = keysym_to_joypad(event.key.keysym.sym)))
                gbx_set_input_state(gt->ctx, input_index, 1);
            break;
        case SDL_KEYUP:
            if (-1 != (input_index = keysym_to_joypad(event.key.keysym.sym)))
                gbx_set_input_state(gt->ctx, input_index, 0);
            break;
        case SDL_QUIT:
            return 0;
        }
    }
    return 1;
}

// -----------------------------------------------------------------------------
int window_event_pump(gbx_thread_t *gt, window_state_t *ws)
{
    ws->gfx = graphics_init(GBX_LCD_XRES, GBX_LCD_YRES, ws->stretch);

    while (process_window_events(gt, ws)) {
        // render the surface and swap buffers
        graphics_render(ws->gfx);
        SDL_GL_SwapWindow(ws->wnd);
    }

    graphics_shutdown(ws->gfx);
    SDL_GL_DeleteContext(ws->glctx);
    SDL_DestroyWindow(ws->wnd);

    return 1;
}

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    cmdargs_t ca;
    gbx_context_t *ctx = NULL;
    gbx_thread_t *gt = NULL;
    window_state_t ws = {0};
    perf_args_t pa = {0};

    // process and validate the command line arguments
    if (cmdline_parse(argc, argv, &ca))
        cmdline_display_usage();

    // create an emulator context for the given system type (default AUTO)
    if (gbx_create_context(&ctx, ca.system)) {
        log_err("Failed to create gbx context.\n");
        goto error_cleanup;
    }

    // set the bios location (if provided) and attempt to load the ROM file
    if (ca.bios_path)
        gbx_set_bios_dir(ctx, ca.bios_path);

    if (gbx_load_file(ctx, ca.rom_path)) {
        log_err("Failed to load image \"%s\".\n", ca.rom_path);
        goto error_cleanup;
    }

    // create and display the application window
    if (create_sdl_window(&ws, &ca))
        goto error_cleanup;

    if (NULL == (gt = gbx_thread_create(ctx, &ca)))
        goto error_cleanup;

    pa.ctx = ctx;
    pa.last_cycles = 0;
    pa.id = SDL_AddTimer(1000, perf_timer_event, (void *)&pa);

    // finally, process window events until the user terminates the program
    window_event_pump(gt, &ws);

error_cleanup:
    SDL_RemoveTimer(pa.id);
    gbx_thread_destroy(gt);
    gbx_destroy_context(ctx);
    cmdline_destroy(&ca);
    return 0;
}

