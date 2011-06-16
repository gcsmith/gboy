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

#include "common.h"
#include "romfile.h"

// LCD screen resolution, identical for all game boy variants (before GBA)

#define GBX_LCD_XRES    160
#define GBX_LCD_YRES    144

// joypad inputs (from frontend)

#define INPUT_RIGHT     0
#define INPUT_LEFT      1
#define INPUT_UP        2
#define INPUT_DOWN      3
#define INPUT_A         4
#define INPUT_B         5
#define INPUT_SELECT    6
#define INPUT_START     7

// memory bank sizes (in bytes)

#define XROM_BANK_SIZE  0x4000  // external ROM - 16 KB
#define XRAM_BANK_SIZE  0x2000  // external RAM - 8 KB
#define VRAM_BANK_SIZE  0x2000  // internal video RAM - 8 KB
#define WRAM_BANK_SIZE  0x1000  // internal work RAM - 4 KB

// memory bank masks (all banks sizes are pow 2)

#define XROM_MASK       (XROM_BANK_SIZE - 1)
#define XRAM_MASK       (XRAM_BANK_SIZE - 1)
#define VRAM_MASK       (VRAM_BANK_SIZE - 1)
#define WRAM_MASK       (WRAM_BANK_SIZE - 1)

// supported system types

#define SYSTEM_GMB      0       // emulate gameboy original
#define SYSTEM_GBP      1       // emulate gameboy pocket
#define SYSTEM_CGB      2       // emulate gameboy color
#define SYSTEM_SGB      3       // emulate super gameboy
#define SYSTEM_SGB2     4       // emulate super gameboy 2
#define SYSTEM_GBA      5       // emulate gameboy advance (detection only)
#define SYSTEM_AUTO     6       // automatic system detection (must be last)

// clock frequencies (in Hz) for each game boy model

#define CPU_FREQ_GMB    4194304
#define CPU_FREQ_GBP    4194304 // same as GMB
#define CPU_FREQ_CGB    8388608 // max speed mode, otherwise same as GMB
#define CPU_FREQ_SGB    4295454 // SNES clock speed divided by 5
#define CPU_FREQ_SGB2   4194304 // corrected from SGB, same as GMB
#define CPU_FREQ_GBA    8388608 // operating in GBC mode (max speed mode)

// cpu flags (register F)

#define FLAG_C          0x10    // carry
#define FLAG_H          0x20    // half-carry
#define FLAG_N          0x40    // subtract
#define FLAG_Z          0x80    // zero

// 8-bit register indices

#define REG_F   0
#define REG_A   1
#define REG_C   2
#define REG_B   3
#define REG_E   4
#define REG_D   5
#define REG_L   6
#define REG_H   7

// 16-bit register pair indices

#define REG_AF  0
#define REG_BC  1
#define REG_DE  2
#define REG_HL  3
#define REG_SP  4
#define REG_PC  5

// interrupt master enable flag

#define IME_DISABLE     0x00
#define IME_ENABLE      0x01

// interrupt flags

#define INT_VBLANK      0x01
#define INT_LCDSTAT     0x02
#define INT_TIMER       0x04
#define INT_SERIAL      0x08
#define INT_JOYPAD      0x10

// interrupt vector addresses

#define INT_VEC_VBLANK  0x40
#define INT_VEC_LCDSTAT 0x48
#define INT_VEC_TIMER   0x50
#define INT_VEC_SERIAL  0x58
#define INT_VEC_JOYPAD  0x60

// execution interruption flags

#define EXEC_BREAK      0x01
#define EXEC_TRACE      0x02
#define EXEC_HALT       0x04
#define EXEC_STOP       0x08

typedef struct cpu_registers {
    union {
        struct {
            union { uint16_t af; struct { uint8_t f, a; }; };
            union { uint16_t bc; struct { uint8_t c, b; }; };
            union { uint16_t de; struct { uint8_t e, d; }; };
            union { uint16_t hl; struct { uint8_t l, h; }; };
            uint16_t sp;
            uint16_t pc;
        };
        uint8_t vb[12];
        uint16_t vw[6];
    };
} cpu_registers_t;

typedef struct timer_registers {
    int div, div_ticks;
    int tima, tima_ticks, tima_limit;
    int tma;
    int tac;
} timer_registers_t;

#define VIDEO_STATE_SEARCH      0
#define VIDEO_STATE_TRANSFER    1
#define VIDEO_STATE_HBLANK      2
#define VIDEO_STATE_VBLANK      3

#define SEARCH_CYCLES           80
#define TRANSFER_CYCLES         172
#define HBLANK_CYCLES           204
#define VBLANK_CYCLES           4560
#define HORIZONTAL_CYCLES       456
#define VERTICAL_CYCLES         70224
#define TOTAL_SCANLINES         154

typedef struct video_registers {
    int state, cycle;
    int lcdc, stat, lyc;
    int lcd_x, lcd_y;
    int scy, scx;
    int wx, wy;
    int bgp, obp0, obp1;
    uint32_t bgp_rgb[4];
    uint32_t obp0_rgb[4];
    uint32_t obp1_rgb[4];
    int bcps, ocps;
    uint8_t bcpd[0x40];
    uint8_t ocpd[0x40];
    uint32_t bcpd_rgb[0x20];
    uint32_t ocpd_rgb[0x20];
} video_registers_t;

typedef struct dma_registers {
    int src;
    int active;
    int cycle;
    int write_pos;
    int write_cycle;
} dma_registers_t;

typedef struct memory_regions {
    uint8_t oam[0xA0];          // object attribute memory
    uint8_t hram[0x80];         // on chip high memory / zero page
    uint8_t *bios;              // (optional) bios image
    uint8_t *xrom, *xrom_bank;  // base address and current bank of ext ROM
    uint8_t *xram, *xram_bank;  // base address and current bank of ext RAM
    uint8_t *vram, *vram_bank;  // base address and current bank of video RAM
    uint8_t *wram, *wram_bank;  // base address and current bank of work RAM
    int xrom_banks, xrom_bnum;
    int xram_banks, xram_bnum;
    int vram_banks, vram_bnum;
    int wram_banks, wram_bnum;
} memory_regions_t;

typedef struct gbx_context {
    rom_header_t header;
    memory_regions_t mem;
    cpu_registers_t reg;
    dma_registers_t dma;
    timer_registers_t timer;
    video_registers_t video;
    const char *bios_dir;
    int ime, ei_delay, di_delay;
    uint8_t int_en;
    uint8_t int_flags, int_flags_delay;
    int system;
    int key1;
    int cart_features;
    int cgb_enabled;
    int sb, sc, sc_active, sc_ticks;
    int have_mbc1, have_mbc2, have_mbc3, have_mbc5;
    int mbc1_mode;
    int cpu_speed;
    int bios_enabled;
    int exec_flags;
    int bytes_read;
    long cycles;
    int joyp, input_state;
    uint8_t opcode1;
    uint8_t opcode2;
    uint16_t next_pc;
    void *userdata;
    uint32_t fb[GBX_LCD_XRES * GBX_LCD_YRES];
} gbx_context_t;

int  gbx_create_context(gbx_context_t **ctx, int system);
void gbx_destroy_context(gbx_context_t *ctx);

int  gbx_load_file(gbx_context_t *ctx, const char *path);
void gbx_power_on(gbx_context_t *ctx);
long gbx_execute_cycles(gbx_context_t *ctx, long cycles);
void gbx_req_interrupt(gbx_context_t *ctx, int interrupt);

void gbx_set_userdata(gbx_context_t *ctx, void *userdata);
void gbx_set_bios_dir(gbx_context_t *ctx, const char *path);
void gbx_set_debugger(gbx_context_t *ctx, int enable);
void gbx_set_input_state(gbx_context_t *ctx, int input, int pressed);

void gbx_get_framebuffer(gbx_context_t *ctx, void *dest);
int  gbx_get_clock_frequency(gbx_context_t *ctx);

int  gbx_disassemble_op(gbx_context_t *ctx, char *buffer, int size);
void gbx_trace_instruction(gbx_context_t *ctx);

void video_update_cycles(gbx_context_t *ctx, long cycles);

extern const int gbx_instruction_cycles[256];
extern const int gbx_instruction_cycles_cb[256];
extern const uint16_t gbx_daa_lut[2048];
extern const uint32_t gbx_monochrome_colors[4];
extern const char *gbx_port_names[0x100];

// interface to emulator frontend

extern void ext_video_sync(void *data);
extern void ext_speed_change(void *data, int speed);
extern void ext_lcd_enabled(void *data, int enabled);

#endif // GBOY_GBX__H

