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

#ifndef GBOY_MEMORY__H
#define GBOY_MEMORY__H

#include "common.h"

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

#define BIOS_UNMAP  0
#define BIOS_MAP    1

// #define PROTECT_OAM_ACCESS
// #define PROTECT_VRAM_ACCESS

typedef uint8_t (*mmu_rd_fn)(gbx_context_t *, uint16_t);
typedef void (*mmu_wr_fn)(gbx_context_t *, uint16_t, uint8_t);

typedef struct memory_regions {
    uint8_t oam[0x100];         // object attribute memory
    uint8_t hram[0x100];        // on chip high memory / zero page
    uint8_t *bios;              // (optional) bios image
    uint8_t *xrom, *xrom_bank;  // base address and current bank of ext ROM
    uint8_t *xram, *xram_bank;  // base address and current bank of ext RAM
    uint8_t *vram, *vram_bank;  // base address and current bank of video RAM
    uint8_t *wram, *wram_bank;  // base address and current bank of work RAM
    int xrom_banks, xrom_bnum;
    int xram_banks, xram_bnum;
    int vram_banks, vram_bnum;
    int wram_banks, wram_bnum;
    mmu_rd_fn page_rd[0x100];
    mmu_wr_fn page_wr[0x100];
    int mbc1_mode;
} memory_regions_t;

void mmu_map_bios(gbx_context_t *ctx, int setting);
void mmu_map_mbc1(gbx_context_t *ctx);
void mmu_map_mbc2(gbx_context_t *ctx);
void mmu_map_mbc3(gbx_context_t *ctx);
void mmu_map_mbc5(gbx_context_t *ctx);
void mmu_map_mbc7(gbx_context_t *ctx);

void mmu_map_pages(gbx_context_t *ctx);
void mmu_map_rw(gbx_context_t *ctx, int beg, int n, mmu_rd_fn rf, mmu_wr_fn wf);
void mmu_map_ro(gbx_context_t *ctx, int beg, int n, mmu_rd_fn fn);
void mmu_map_wo(gbx_context_t *ctx, int beg, int n, mmu_wr_fn fn);

uint8_t gbx_read_byte(gbx_context_t *ctx, uint16_t addr);
void    gbx_write_byte(gbx_context_t *ctx, uint16_t addr, uint8_t data);

// -----------------------------------------------------------------------------
INLINE uint16_t gbx_read_word(gbx_context_t *ctx, uint16_t addr)
{
    return gbx_read_byte(ctx, addr) | gbx_read_byte(ctx, addr + 1) << 8;
}

// -----------------------------------------------------------------------------
INLINE void gbx_write_word(gbx_context_t *ctx, uint16_t addr, uint16_t data)
{
    gbx_write_byte(ctx, addr, data & 0xFF);
    gbx_write_byte(ctx, addr + 1, data >> 8);
}

#endif // GBOY_MEMORY__H

