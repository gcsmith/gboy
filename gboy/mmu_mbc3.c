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

// ----------------------------------------------------------------------------
void mmu_wr_mbc3_ramg(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("MBC3 set RAM enable, addr:%04X data:%02X\n", addr, value);
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc3_rtc(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_err("TODO: implement RTC registers\n");
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc3_romb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    // specify a full 7-bit ROM bank index, but 0 always maps to 1
    int bank = set_xrom_bank(ctx, (value & 0x7F) ? (value & 0x7F) : 1);
    log_spew("MBC3 set XROM bank %02X (set bits %02X)\n", bank, value);
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc3_ramb(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    if (value <= 0x03) {
        if (ctx->mem.xram_banks) {
            int bank = set_xram_bank(ctx, value & 0x03);
            log_spew("MBC3 set XRAM bank %02X (set bits %02X)\n", bank,  value);
        }
        else
            log_warn("MBC3 set XRAM bank %02X, but no XRAM present\n", value);
    }
    else {
        log_err("TODO: implement RTC register select\n");
    }
}

// ----------------------------------------------------------------------------
void mmu_wr_mbc3_latch(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_err("TODO: implement latch clock data\n");
}

// ----------------------------------------------------------------------------
void mmu_map_mbc3(gbx_context_t *ctx)
{
    mmu_map_wo(ctx, 0x00, 0x20, mmu_wr_mbc3_ramg);
    mmu_map_wo(ctx, 0x20, 0x20, mmu_wr_mbc3_romb);
    mmu_map_wo(ctx, 0x40, 0x20, mmu_wr_mbc3_ramb);
    mmu_map_wo(ctx, 0x60, 0x20, mmu_wr_mbc3_latch);
}

