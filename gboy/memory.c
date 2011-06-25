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
#include "gbx.h"
#include "memory.h"
#include "memory_util.h"
#include "ports.h"
#include "video.h"

// ----------------------------------------------------------------------------
uint8_t mmu_rd_invalid(gbx_context_t *ctx, uint16_t addr)
{
    log_warn("reading from unmapped page: addr=%04X value=FF\n", addr);
    return 0xFF;
}

// ----------------------------------------------------------------------------
void mmu_wr_invalid(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_warn("writing to unmapped page: addr=%04X value=%02X\n", addr, value);
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_bios(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.bios[addr];
    log_spew("mmu_rd_bios: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_xrom(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xrom[addr & XROM_MASK];
    log_spew("mmu_rd_xrom: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_xrom_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xrom_bank[addr & XROM_MASK];
    log_spew("mmu_rd_xrom_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_xram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xram_bank[addr & XRAM_MASK];
    log_spew("mmu_rd_xram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
void mmu_wr_xram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_xram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.xram_bank[addr & XRAM_MASK] = value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_vram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.vram_bank[addr & VRAM_MASK];
#ifdef PROTECT_VRAM_ACCESS
    if (!is_vram_accessible(ctx)) {
        log_warn("attempted to read VRAM when in use by LCD controller\n");
        return 0xFF;
    }
#endif

    log_spew("mmu_rd_vram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
void mmu_wr_vram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
#ifdef PROTECT_VRAM_ACCESS
    if (!is_vram_accessible(ctx)) {
        log_warn("attempted to write VRAM when in use by LCD controller\n");
        return;
    }
#endif

    log_spew("mmu_wr_vram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.vram_bank[addr & VRAM_MASK] = value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_wram(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.wram[addr & WRAM_MASK];
    log_spew("mmu_rd_wram: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
void mmu_wr_wram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_wram: addr=%04X value=%02X\n", addr, value);
    ctx->mem.wram[addr & WRAM_MASK] = value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_wram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.wram_bank[addr & WRAM_MASK];
    log_spew("mmu_rd_wram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
void mmu_wr_wram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_wram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.wram_bank[addr & WRAM_MASK] = value;
}

// ----------------------------------------------------------------------------
uint8_t mmu_rd_oam(gbx_context_t *ctx, uint16_t addr)
{
#ifdef PROTECT_OAM_ACCESS
    if (!is_oam_accessible(ctx)) {
        log_warn("attempted to read OAM when in use by LCD controller\n");
        return 0xFF;
    }

    if (addr > 0xFE9F) {
        log_warn("attempting to read unusable memory region (%04X)\n", addr);
        return 0xFF;
    }
#endif

    log_spew("mmu_rd_oam: addr=%04X\n", addr);
    return ctx->mem.oam[addr & 0xFF];
}

// ----------------------------------------------------------------------------
void mmu_wr_oam(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
#ifdef PROTECT_OAM_ACCESS
    if (!is_oam_accessible(ctx)) {
        log_warn("attempted to write OAM when in use by LCD controller\n");
        return;
    }

    if (addr > 0xFE9F) {
        log_warn("attempting to write unusable memory region (%04X)\n", addr);
        return;
    }
#endif

    log_spew("mmu_wr_oam: addr=%04X value=%02X\n", addr, value);
    ctx->mem.oam[addr & 0xFF] = value;
}

// ----------------------------------------------------------------------------
INLINE uint8_t read_joyp_port(gbx_context_t *ctx)
{
    uint8_t input_bits = 0x0F;
    if (!(ctx->joyp & JOYP_SEL_DIR))
        input_bits = ctx->input_state & 0xF;
    else if (!(ctx->joyp & JOYP_SEL_BTN))
        input_bits = ctx->input_state >> 4;
    return ctx->joyp | input_bits | 0xC0;
}

// ----------------------------------------------------------------------------
INLINE void write_timer_control(gbx_context_t *ctx, uint8_t value)
{
    switch (value & 3) {
    case 0: ctx->timer.tima_limit = 1024; break; // 4096 Hz
    case 1: ctx->timer.tima_limit =   16; break; // 262144 Hz
    case 2: ctx->timer.tima_limit =   64; break; // 65536 Hz
    case 3: ctx->timer.tima_limit =  256; break; // 16384 Hz
    }
    ctx->timer.tac = value & 7;
}

// ----------------------------------------------------------------------------
INLINE void start_dma_transfer(gbx_context_t *ctx, uint8_t value)
{
    if (ctx->dma.active) {
        log_warn("attempting to start new DMA transfer with DMA active\n");
        return;
    }

    if (value > 0xF1) {
        log_warn("specified DMA source address above F19F (%02X00)\n", value);
        value = 0xF1;
    }

    ctx->dma.src = value << 8;
    ctx->dma.active = 1;
    ctx->dma.cycle = 0;
    ctx->dma.write_pos = 0;
    ctx->dma.write_cycle = 0;

    log_dbg("begin A0 byte DMA transfer from %04X to FE00\n", ctx->dma.src);
}

// ----------------------------------------------------------------------------
static void write_infrared_comm_port(gbx_context_t *ctx, uint8_t value)
{
    log_dbg("write %02X to IR comm port\n", value);
    ctx->rp = value & (RP_WR_DATA | RP_RD_ENABLE);
}

// ----------------------------------------------------------------------------
static uint8_t read_infrared_comm_port(gbx_context_t *ctx)
{
    log_dbg("read %02X from IR comm port\n", 0);
    return ctx->rp | RP_RD_DATA;
}

// ----------------------------------------------------------------------------
static uint8_t mmu_rd_himem(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = 0;
    int offset = addr & 0xFF;

    switch (offset) {
    case PORT_JOYP:
        value = read_joyp_port(ctx);
        break;
    case PORT_SB:
        value = 0xFF; //ctx->sb;
        break;
    case PORT_SC:
        value = ctx->sc;
        break;
    case PORT_DIV:
        value = ctx->timer.div;
        break;
    case PORT_TIMA:
        value = ctx->timer.tima;
        break;
    case PORT_TMA:
        value = ctx->timer.tma;
        break;
    case PORT_TAC:
        value = ctx->timer.tac;
        break;
    case PORT_IF:
        value = ctx->int_flags | ~INT_MASK; // set unused bits
        break;
    case PORT_IE:
        value = ctx->int_en;
        break;
    case PORT_LCDC:
        value = ctx->video.lcdc;
        break;
    case PORT_STAT:
        value = video_read_stat(ctx);
        break;
    case PORT_SCY:
        value = ctx->video.scy;
        break;
    case PORT_SCX:
        value = ctx->video.scx;
        break;
    case PORT_LY:
        value = ctx->video.lcd_y;
        break;
    case PORT_LYC:
        value = ctx->video.lyc;
        break;
    case PORT_DMA:
        value = ctx->dma.src >> 8; // ???
        break;
    case PORT_BGP:
        value = ctx->video.bgp;
        break;
    case PORT_OBP0:
        value = ctx->video.obp0;
        break;
    case PORT_OBP1:
        value = ctx->video.obp1;
        break;
    case PORT_WY:
        value = ctx->video.wy;
        break;
    case PORT_WX:
        value = ctx->video.wx + 7;
        break;
    case PORT_KEY1:
        value = ctx->key1;
        break;
    case PORT_VBK:
        value = ctx->mem.vram_bnum;
        break;
    case PORT_HDMA1:
        value = ctx->video.hdma_src >> 8;
        break;
    case PORT_HDMA2:
        value = ctx->video.hdma_src & 0xFF;
        break;
    case PORT_HDMA3:
        value = ctx->video.hdma_dst >> 8;
        break;
    case PORT_HDMA4:
        value = ctx->video.hdma_dst & 0xFF;
        break;
    case PORT_HDMA5:
        value = video_read_hdma(ctx);
        break;
    case PORT_RP:
        value = read_infrared_comm_port(ctx);
        break;
    case PORT_BCPS:
        return ctx->video.bcps;
    case PORT_BCPD:
        return ctx->video.bcpd[ctx->video.bcps & CPS_INDEX];
    case PORT_OCPS:
        return ctx->video.ocps;
    case PORT_OCPD:
        return ctx->video.ocpd[ctx->video.ocps & CPS_INDEX];
    case PORT_SVBK:
        value = ctx->mem.wram_bnum;
        break;
    case PORT_NR10:
    case PORT_NR11:
    case PORT_NR12:
    case PORT_NR13:
    case PORT_NR14:
    case PORT_NR21:
    case PORT_NR22:
    case PORT_NR23:
    case PORT_NR24:
    case PORT_NR30:
    case PORT_NR31:
    case PORT_NR32:
    case PORT_NR33:
    case PORT_NR34:
    case PORT_WAV0: case PORT_WAV1: case PORT_WAV2: case PORT_WAV3:
    case PORT_WAV4: case PORT_WAV5: case PORT_WAV6: case PORT_WAV7:
    case PORT_WAV8: case PORT_WAV9: case PORT_WAVA: case PORT_WAVB:
    case PORT_WAVC: case PORT_WAVD: case PORT_WAVE: case PORT_WAVF:
    case PORT_NR41:
    case PORT_NR42:
    case PORT_NR43:
    case PORT_NR44:
    case PORT_NR50:
    case PORT_NR51:
    case PORT_NR52:
    case PORT_NR_UNK:
        ext_sound_read(ctx->userdata, addr, &value);
        break;
    case PORT_BIOS:
        value = ctx->bios_enabled;
        break;
    default:
        if (offset < 0x80)
            log_warn("attempting to read invalid io port (%04X)\n", addr);
        value = ctx->mem.hram[offset];
        break;
    }

    log_spew("mmu_rd_himem: addr=%04X value=%02X\n", addr, value);
    return value;
}

// ----------------------------------------------------------------------------
static void mmu_wr_himem(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    uint16_t dma_addr;
    int offset = addr & 0xFF;

    log_spew("mmu_wr_himem: addr=%04X value=%02X\n", addr, value);

    switch (offset) {
    case PORT_JOYP:
        // only the button/direction select bits (P14/P15) are writable
        ctx->joyp = value & (JOYP_SEL_DIR | JOYP_SEL_BTN);
        break;
    case PORT_SB:
        ctx->sb = value;
        break;
    case PORT_SC:
        ctx->sc = value & 0x83;
        if ((value & SC_XFER_START) && (value & SC_CLK_SHIFT)) {
            ctx->sc_active = 1;
            ctx->sc_ticks = 0;
        }
        break;
    case PORT_DIV:
        // writing to DIV always resets it to 0, regardless of the value
        ctx->timer.div = 0;
        ctx->timer.div_ticks = ctx->cycles % 256;
        break;
    case PORT_TIMA:
        ctx->timer.tima = value;
        break;
    case PORT_TMA:
        ctx->timer.tma = value;
        break;
    case PORT_TAC:
        write_timer_control(ctx, value);
        ctx->timer.tima_ticks = ctx->cycles % ctx->timer.tima_limit;
        break;
    case PORT_IF:
        ctx->int_flags = value & INT_MASK;
        break;
    case PORT_IE:
        ctx->int_en = value & INT_MASK;
        break;
    case PORT_LCDC:
        video_write_lcdc(ctx, value);
        break;
    case PORT_STAT:
        video_write_stat(ctx, value);
        break;
    case PORT_SCY:
        ctx->video.scy = value;
        break;
    case PORT_SCX:
        ctx->video.scx = value;
        break;
    case PORT_LY:
        log_warn("attempting to write read-only PORT_LY (%02X)\n", value);
        break;
    case PORT_LYC:
        ctx->video.lyc = value;
        break;
    case PORT_DMA:
        start_dma_transfer(ctx, value);
        break;
    case PORT_BGP:
        ctx->video.bgp = value;
        video_write_mono_palette(ctx->video.bgp_rgb, value);
        break;
    case PORT_OBP0:
        ctx->video.obp0 = value;
        video_write_mono_palette(ctx->video.obp0_rgb, value);
        break;
    case PORT_OBP1:
        ctx->video.obp1 = value;
        video_write_mono_palette(ctx->video.obp1_rgb, value);
        break;
    case PORT_WY:
        ctx->video.wy = value;
        break;
    case PORT_WX:
        ctx->video.wx = value - 7;
        break;
    case PORT_KEY1:
        // only prep bit is writeable, mask out read-only / unused bits
        ctx->key1 = (ctx->key1 & ~KEY1_PREP) | (value & KEY1_PREP);
        break;
    case PORT_VBK:
        set_vram_bank(ctx, value);
        break;
    case PORT_HDMA1:
        dma_addr = (ctx->video.hdma_src & 0x00FF) | (value << 8);
        ctx->video.hdma_src = video_validate_hdma_src(dma_addr);
        break;
    case PORT_HDMA2:
        dma_addr = (ctx->video.hdma_src & 0xFF00) | value;
        ctx->video.hdma_src = video_validate_hdma_src(dma_addr);
        break;
    case PORT_HDMA3:
        dma_addr = (ctx->video.hdma_dst & 0x00FF) | (value << 8);
        ctx->video.hdma_dst = video_validate_hdma_dst(dma_addr);
        break;
    case PORT_HDMA4:
        dma_addr = (ctx->video.hdma_dst & 0xFF00) | value;
        ctx->video.hdma_dst = video_validate_hdma_dst(dma_addr);
        break;
    case PORT_HDMA5:
        video_write_hdma(ctx, value);
        break;
    case PORT_RP:
        write_infrared_comm_port(ctx, value);
        break;
    case PORT_BCPS:
        ctx->video.bcps = value & 0xBF;
        break;
    case PORT_BCPD:
        video_write_bcpd(ctx, value);
        break;
    case PORT_OCPS:
        ctx->video.ocps = value & 0xBF;
        break;
    case PORT_OCPD:
        video_write_ocpd(ctx, value);
        break;
    case PORT_SVBK:
        set_wram_bank(ctx, value);
        break;
    case PORT_NR10:
    case PORT_NR11:
    case PORT_NR12:
    case PORT_NR13:
    case PORT_NR14:
    case PORT_NR21:
    case PORT_NR22:
    case PORT_NR23:
    case PORT_NR24:
    case PORT_NR30:
    case PORT_NR31:
    case PORT_NR32:
    case PORT_NR33:
    case PORT_NR34:
    case PORT_WAV0: case PORT_WAV1: case PORT_WAV2: case PORT_WAV3:
    case PORT_WAV4: case PORT_WAV5: case PORT_WAV6: case PORT_WAV7:
    case PORT_WAV8: case PORT_WAV9: case PORT_WAVA: case PORT_WAVB:
    case PORT_WAVC: case PORT_WAVD: case PORT_WAVE: case PORT_WAVF:
    case PORT_NR41:
    case PORT_NR42:
    case PORT_NR43:
    case PORT_NR44:
    case PORT_NR50:
    case PORT_NR51:
    case PORT_NR52:
    case PORT_NR_UNK:
        ext_sound_write(ctx->userdata, addr, value);
        break;
    case PORT_BIOS:
        if (ctx->bios_enabled) {
            // unmap the bios page(s) permanently
            mmu_map_bios(ctx, BIOS_UNMAP);
            ctx->bios_enabled = 0;

            // if running monochrome game on CGB, disable color post-bios
            ctx->color_enabled = ctx->color_game;
        }
        break;
    default:
        if (offset < 0x80)
            log_warn("write to invalid io port (%04X) <- %02X\n", addr, value);
        ctx->mem.hram[offset] = value;
        return;
    }
}

// ----------------------------------------------------------------------------
void mmu_map_bios(gbx_context_t *ctx, int setting)
{
    mmu_rd_fn fn = (setting == BIOS_MAP) ? mmu_rd_bios : mmu_rd_xrom;

    if (ctx->system == SYSTEM_CGB) {
        // for the CGB, the bios image is mapped to 0000-00FF and 0200-08FF
        mmu_map_ro(ctx, 0x00, 1, fn);
        mmu_map_ro(ctx, 0x02, 7, fn);
    }
    else {
        // for the DMG/GBP/SGB, the bios image is a single page in length
        mmu_map_ro(ctx, 0x00, 1, fn);
    }
}

// ----------------------------------------------------------------------------
void mmu_map_pages(gbx_context_t *ctx)
{
    // XROM is read-only (may be altered by MBC settings), VRAM always banked
    mmu_map_wo(ctx, 0x00, 0x80, mmu_wr_invalid);
    mmu_map_ro(ctx, 0x00, 0x40, mmu_rd_xrom);
    mmu_map_ro(ctx, 0x40, 0x40, mmu_rd_xrom_bank);
    mmu_map_rw(ctx, 0x80, 0x20, mmu_rd_vram_bank, mmu_wr_vram_bank);

    // map the external RAM handlers only if RAM present, unmapped otherwise
    if (ctx->mem.xram_banks)
        mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_xram_bank, mmu_wr_xram_bank);
    else
        mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_invalid, mmu_wr_invalid);

    // set RW handlers for WRAM, WRAM mirror, OAM, IO ports, and HRAM
    mmu_map_rw(ctx, 0xC0, 0x10, mmu_rd_wram, mmu_wr_wram);
    mmu_map_rw(ctx, 0xD0, 0x10, mmu_rd_wram_bank, mmu_wr_wram_bank);
    mmu_map_rw(ctx, 0xE0, 0x10, mmu_rd_wram, mmu_wr_wram);
    mmu_map_rw(ctx, 0xF0, 0x0E, mmu_rd_wram_bank, mmu_wr_wram_bank);
    mmu_map_rw(ctx, 0xFE, 0x01, mmu_rd_oam, mmu_wr_oam);
    mmu_map_rw(ctx, 0xFF, 0x01, mmu_rd_himem, mmu_wr_himem);

    // configure memory bank controller specific mappings
    switch (ctx->cart_features & CART_MBC) {
    default:
        // shouldn't ever get here... but fall through just in case
        assert(!"unknown memory bank controller in mmu_map_pages");
    case CART_MBC_ROM: break;
    case CART_MBC_MBC1: mmu_map_mbc1(ctx); break;
    case CART_MBC_MBC2: mmu_map_mbc2(ctx); break;
    case CART_MBC_MBC3: mmu_map_mbc3(ctx); break;
    case CART_MBC_MBC5: mmu_map_mbc5(ctx); break;
    case CART_MBC_MBC7: mmu_map_mbc7(ctx); break;
    }

    // if a bios ROM image is available, patch it into the memory map
    if (ctx->bios_enabled)
        mmu_map_bios(ctx, BIOS_MAP);
}

// ----------------------------------------------------------------------------
void mmu_map_ro(gbx_context_t *ctx, int beg, int n, mmu_rd_fn fn)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++)
        ctx->mem.page_rd[page] = fn;
}

// ----------------------------------------------------------------------------
void mmu_map_wo(gbx_context_t *ctx, int beg, int n, mmu_wr_fn fn)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++)
        ctx->mem.page_wr[page] = fn;
}

// ----------------------------------------------------------------------------
void mmu_map_rw(gbx_context_t *ctx, int beg, int n, mmu_rd_fn rf, mmu_wr_fn wf)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++) {
        ctx->mem.page_rd[page] = rf;
        ctx->mem.page_wr[page] = wf;
    }
}

// ----------------------------------------------------------------------------
uint8_t gbx_read_byte(gbx_context_t *ctx, uint16_t addr)
{
    return ctx->mem.page_rd[addr >> 8](ctx, addr);
}

// ----------------------------------------------------------------------------
void gbx_write_byte(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    ctx->mem.page_wr[addr >> 8](ctx, addr, value);
}

