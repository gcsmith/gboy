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

#ifndef GBOY_GBX__H
#define GBOY_GBX__H

#include "cpu.h"
#include "memory.h"
#include "romfile.h"
#include "video.h"

// joypad inputs (from frontend)

#define INPUT_RIGHT     0
#define INPUT_LEFT      1
#define INPUT_UP        2
#define INPUT_DOWN      3
#define INPUT_A         4
#define INPUT_B         5
#define INPUT_SELECT    6
#define INPUT_START     7

// supported system types

#define SYSTEM_DMG      0       // emulate gameboy original
#define SYSTEM_GBP      1       // emulate gameboy pocket
#define SYSTEM_CGB      2       // emulate gameboy color
#define SYSTEM_SGB      3       // emulate super gameboy
#define SYSTEM_SGB2     4       // emulate super gameboy 2
#define SYSTEM_GBA      5       // emulate gameboy advance (detection only)
#define SYSTEM_AUTO     6       // automatic system detection (must be last)

// clock frequencies (in Hz) for each game boy model

#define CPU_FREQ_DMG    4194304
#define CPU_FREQ_GBP    4194304 // same as DMG
#define CPU_FREQ_CGB    8388608 // max speed mode, otherwise same as DMG
#define CPU_FREQ_SGB    4295454 // SNES clock speed divided by 5
#define CPU_FREQ_SGB2   4194304 // corrected from SGB, same as DMG
#define CPU_FREQ_GBA    8388608 // operating in GBC mode (max speed mode)

// execution interruption flags

#define EXEC_BREAK      0x01
#define EXEC_TRACE      0x02
#define EXEC_HALT       0x04
#define EXEC_STOP       0x08

struct gbx_context {
    memory_regions_t mem;
    cpu_registers_t reg;
    dma_registers_t dma;
    timer_registers_t timer;
    video_registers_t video;
    const char *bios_dir;
    uint8_t int_en, int_flags, int_flags_delay;
    int ime, ei_delay, di_delay;
    int system;
    int key1, fast_mode;
    int cart_features;
    int color_enabled;
    int color_game;
    int sb, sc, sc_active, sc_ticks;
    int cpu_speed;
    int bios_enabled;
    int exec_flags;
    int bytes_read;
    long cycles, cycle_delta, frame_cycles;
    int joyp, input_state;
    uint8_t rp;
    uint8_t opcode1;
    uint8_t opcode2;
    uint16_t next_pc;
    uint32_t fb[GBX_LCD_XRES * GBX_LCD_YRES];
    void *userdata;
    FILE *serial_log;
};

int  gbx_create_context(gbx_context_t **ctx, int system);
void gbx_destroy_context(gbx_context_t *ctx);

int  gbx_load_file(gbx_context_t *ctx, const char *path);
void gbx_power_on(gbx_context_t *ctx);
long gbx_execute_cycles(gbx_context_t *ctx, long cycles);
void gbx_req_interrupt(gbx_context_t *ctx, int interrupt);

void gbx_set_userdata(gbx_context_t *ctx, void *userdata);
void gbx_set_bios_dir(gbx_context_t *ctx, const char *path);
void gbx_set_serial_log(gbx_context_t *ctx, const char *path);
void gbx_set_debugger(gbx_context_t *ctx, int enable);
void gbx_set_input_state(gbx_context_t *ctx, int input, int pressed);

void gbx_get_framebuffer(gbx_context_t *ctx, uint32_t *dest);
void gbx_get_tile_buffer(gbx_context_t *ctx, uint32_t *dest, int index);
void gbx_get_tmap_buffer(gbx_context_t *ctx, uint32_t *dest, int index);
int  gbx_get_clock_frequency(gbx_context_t *ctx);

int  gbx_disassemble_op(gbx_context_t *ctx, char *buffer, int size);
void gbx_trace_instruction(gbx_context_t *ctx);

// interface to emulator frontend

extern void ext_video_sync(void *data);
extern void ext_speed_change(void *data, int speed);
extern void ext_lcd_enabled(void *data, int enabled);
extern void ext_sound_write(void *data, uint16_t addr, uint8_t value);
extern void ext_sound_read(void *data, uint16_t addr, uint8_t *value);

#endif // GBOY_GBX__H

