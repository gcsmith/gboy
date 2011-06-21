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

// ----------------------------------------------------------------------------
void mmu_wr_mbc2_ramg(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("MBC2 set RAM enable, addr:%04X data:%02X\n", addr, value);
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc2_romb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    // specify 4-bit ROM bank index, but 0 always maps to 1
    int bank = set_xrom_bank(ctx, (value & 0x0F) ? (value & 0x0F) : 1);
    log_spew("MBC2 set XROM bank %02X (set bits %02X)\n", bank, value);
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_mbc2_ram(gbx_context_t *ctx, uint16_t addr)
{
    return ctx->mem.xram[addr & 0x1FF];
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc2_ram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    ctx->mem.xram[addr & 0x1FF] = value & 0x0F;
}

// ----------------------------------------------------------------------------
void mmu_map_mbc2(gbx_context_t *ctx)
{
    // MBC2 has only 512 bytes of internal RAM
    ctx->mem.xram = calloc(1, 0x200);

    mmu_map_wo(ctx, 0x00, 0x20, mmu_wr_mbc2_ramg);
    mmu_map_wo(ctx, 0x20, 0x20, mmu_wr_mbc2_romb);
    mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_mbc2_ram, mmu_wr_mbc2_ram);
}

