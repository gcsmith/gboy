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

#include <stdlib.h>
#include <assert.h>
#include "memory.h"
#include "memory_util.h"

#define MBC7_UNK1       0x00
#define MBC7_UNK2       0x10
#define MBC7_XPOS_LO    0x20
#define MBC7_XPOS_HI    0x30
#define MBC7_YPOS_LO    0x40
#define MBC7_YPOS_HI    0x50
#define MBC7_UNK3       0x60
#define MBC7_UNK4       0x70
#define MBC7_UNK5       0x80

// ----------------------------------------------------------------------------
void mmu_wr_mbc7_ramg(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("MBC7 set RAM enable, addr:%04X data:%02X\n", addr, value);
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc7_romb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    // specify 7-bit ROM bank index, but 0 always maps to 1
    int bank = set_xrom_bank(ctx, (value & 0x7F) ? (value & 0x7F) : 1);
    log_spew("MBC7 set XROM bank %02X (set bits %02X)\n", bank, value);
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc7_ramb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    if (ctx->mem.xram_banks) {
        int bank = set_xram_bank(ctx, value & 0x03);
        log_spew("MBC7 set XRAM bank %02X (set bits %02X)\n", bank,  value);
    }
    else
        log_warn("MBC7 set XRAM bank %02X, but no XRAM present\n", value);
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_mbc7_ram(gbx_context_t *ctx, uint16_t addr)
{
    log_info("reading MBC7 ram, addr:%04X\n", addr);
    switch (addr) {
    default:
    case MBC7_UNK1:
    case MBC7_UNK2:
    case MBC7_UNK3:
    case MBC7_UNK4:
    case MBC7_UNK5:
        return 0xFF;
    case MBC7_XPOS_LO:
    case MBC7_XPOS_HI:
    case MBC7_YPOS_LO:
    case MBC7_YPOS_HI:
        return 0;
    }
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc7_ram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_info("writing MBC7 ram, addr:%04X value:%02X\n", addr, value);
}

// ----------------------------------------------------------------------------
void mmu_map_mbc7(gbx_context_t *ctx)
{
    mmu_map_wo(ctx, 0x00, 0x20, mmu_wr_mbc7_ramg);
    mmu_map_wo(ctx, 0x20, 0x20, mmu_wr_mbc7_romb);
    mmu_map_wo(ctx, 0x40, 0x20, mmu_wr_mbc7_ramb);
    mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_mbc7_ram, mmu_wr_mbc7_ram);
}

