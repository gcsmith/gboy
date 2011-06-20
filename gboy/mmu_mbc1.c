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

#include <assert.h>
#include "memory.h"
#include "memory_util.h"

// -----------------------------------------------------------------------------
void mmu_wr_mbc1_ramg(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("MBC1 set RAM enable, addr:%04X data:%02X\n", addr, value);
}

// -----------------------------------------------------------------------------
void mmu_wr_mbc1_romb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    if (ctx->mem.mbc1_mode) {
        int bank = set_xrom_bank(ctx, (value & 0x1F) ? (value & 0x1F) : 1);
        log_spew("MBC1 set XROM bank %02X (set bits %02X)\n", bank, value);
    }
    else {
        // cannot map bank 0 to programmable region (0x00, 0x20, 0x40, 0x60)
        int bank_lo = (value & 0x1F) ? (value & 0x1F) : 1;
        int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0xE0) | bank_lo);
        log_spew("MBC1 set XROM bank %02X (set lo bits %02X)\n", bank, value);
    }
}

// -----------------------------------------------------------------------------
void mmu_wr_mbc1_ramb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    if (ctx->mem.mbc1_mode)  {
        // set the two bit RAM address, assuming XRAM is actually present
        if (ctx->mem.xram_banks) {
            int bank = set_xram_bank(ctx, value & 3);
            log_spew("MBC1 set XRAM bank %02X (set bits %02X)\n", bank, value);
        }
        else
            log_spew("MBC1 set XRAM bank %02X, but no XRAM present\n", value);
    }
    else {
        // set bits 5 and 6 of the XROM bank index when in mode 00
        int bank_hi = (value & 3) << 5;
        int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0x1F) | bank_hi);
        assert(ctx->mem.xrom_bnum & 0x1F);
        log_spew("MBC1 set XROM bank %02X (set hi bits %02X)\n", bank, value);
    }
}

// -----------------------------------------------------------------------------
void mmu_wr_mbc1_mode(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    if (value & 1) {
        // RAM banking mode (4 Mb ROM / 32 KB RAM)
        ctx->mem.mbc1_mode = 1;
    }
    else {
        // ROM banking mode (16 Mb ROM / 8 KB RAM)
        ctx->mem.mbc1_mode = 0;
    }
    log_spew("MBC1 set memory model to %d\n", ctx->mem.mbc1_mode);
}

// -----------------------------------------------------------------------------
void mmu_map_mbc1(gbx_context_t *ctx)
{
    mmu_map_wo(ctx, 0x00, 0x20, mmu_wr_mbc1_ramg);
    mmu_map_wo(ctx, 0x20, 0x20, mmu_wr_mbc1_romb);
    mmu_map_wo(ctx, 0x40, 0x20, mmu_wr_mbc1_ramb);
    mmu_map_wo(ctx, 0x60, 0x20, mmu_wr_mbc1_mode);
}

