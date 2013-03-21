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
uint8_t mmu_rd_pcam_ram(gbx_context_t *ctx, uint16_t addr)
{
    log_info("rd_pcam_ram %04X\n", addr);

    if (addr == 0xA000) {
        return 0;
    }
    else {
        return ctx->mem.xram_bank[addr & 0x1FFF];
    }
}

// ----------------------------------------------------------------------------
void mmu_wr_pcam_ram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_info("wr_pcam_ram %04X <-- %02X\n", addr, value);

    if (addr == 0xA000) {
    }
    else {
        ctx->mem.xram_bank[addr & 0x1FFF] = value;
    }
}

// ----------------------------------------------------------------------------
void mmu_map_pcam(gbx_context_t *ctx)
{
    mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_pcam_ram, mmu_wr_pcam_ram);

#if 0
    // MBC2 has only 512 bytes of internal RAM
    ctx->mem.xram = calloc(1, 0x200);

    mmu_map_wo(ctx, 0x00, 0x20, mmu_wr_mbc2_ramg);
    mmu_map_wo(ctx, 0x20, 0x20, mmu_wr_mbc2_romb);
    mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_mbc2_ram, mmu_wr_mbc2_ram);

    // disable access to external RAM until RAMG is explicitly written to
    ctx->mem.ramg_en = 0;
#endif
}

