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

#ifndef GBOY_MEMORY_UTIL__H
#define GBOY_MEMORY_UTIL__H

#include "gbx.h"

// ----------------------------------------------------------------------------
// Returns a non-zero value if VRAM is currently accessible by the CPU.
INLINE int is_vram_accessible(gbx_context_t *ctx)
{
    int mode = ctx->video.stat & STAT_MODE;
    return (mode < MODE_TRANSFER) ? 1 : 0;
}

// ----------------------------------------------------------------------------
// Returns a non-zero value if OAM is currently accessible by the CPU.
INLINE int is_oam_accessible(gbx_context_t *ctx)
{
    int mode = ctx->video.stat & STAT_MODE;
    return (mode < MODE_SEARCH) ? 1 : 0;
}

// ----------------------------------------------------------------------------
INLINE int set_xrom_bank(gbx_context_t *ctx, int bank)
{
    assert(ctx->mem.xrom_banks);

    // if bank index exceeds number of banks, wrap around to valid index
    if (bank >= ctx->mem.xrom_banks) {
        log_err("specified invalid XROM bank %d, wrapped to %d\n",
                bank, bank % ctx->mem.xrom_banks);
        bank %= ctx->mem.xrom_banks;
    }

    ctx->mem.xrom_bank = ctx->mem.xrom + bank * XROM_BANK_SIZE;
    ctx->mem.xrom_bnum = bank;
    return bank;
}

// ----------------------------------------------------------------------------
INLINE int set_xram_bank(gbx_context_t *ctx, int bank)
{
    assert(ctx->mem.xram_banks);

    // if bank index exceeds number of banks, wrap around to valid index
    if (bank >= ctx->mem.xram_banks) {
        log_err("specified invalid XRAM bank %d, wrapped to %d\n",
                bank, bank % ctx->mem.xram_banks);
        bank %= ctx->mem.xram_banks;
    }

    ctx->mem.xram_bank = ctx->mem.xram + bank * XRAM_BANK_SIZE;
    ctx->mem.xram_bnum = bank;
    return bank;
}

// ----------------------------------------------------------------------------
INLINE void set_vram_bank(gbx_context_t *ctx, uint8_t value)
{
    int bank = value & 1;
    if (!ctx->color_enabled) {
        log_warn("cannot set VRAM bank when not in CGB mode\n");
        return;
    }

    ctx->mem.vram_bank = ctx->mem.vram + bank * VRAM_BANK_SIZE;
    ctx->mem.vram_bnum = bank;
    log_spew("CGB set VRAM bank %02X (set bits %02X)\n", bank,  value);
}

// ----------------------------------------------------------------------------
INLINE void set_wram_bank(gbx_context_t *ctx, uint8_t value)
{
    int bank = (value & 7) ? (value & 7) : 1;
    if (!ctx->color_enabled) {
        log_warn("cannot set WRAM bank when not in CGB mode\n");
        return;
    }

    ctx->mem.wram_bank = ctx->mem.wram + bank * WRAM_BANK_SIZE;
    ctx->mem.wram_bnum = bank;
    log_spew("CGB set WRAM bank %02X (set bits %02X)\n", bank,  value);
}

#endif // GBOY_MEMORY_UTIL__H

