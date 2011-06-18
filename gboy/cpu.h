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

#ifndef GBOY_CPU__H
#define GBOY_CPU__H

#include "common.h"

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
    int div;
    int tima;
    int tma;
    int tac;
    int div_ticks;
    int tima_ticks, tima_limit;
} timer_registers_t;

typedef struct dma_registers {
    int src;
    int active;
    int cycle;
    int write_pos;
    int write_cycle;
} dma_registers_t;

#endif // GBOY_CPU__H

