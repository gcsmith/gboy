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

// #define PROTECT_OAM_ACCESS
// #define PROTECT_VRAM_ACCESS

// -----------------------------------------------------------------------------
uint8_t mmu_rd_nop(gbx_context_t *ctx, uint16_t addr)
{
    log_err("reading from unmapped page: addr=%04X value=FF\n", addr);
    return 0xFF;
}

// -----------------------------------------------------------------------------
void mmu_wr_nop(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_err("writing to unmapped page: addr=%04X value=%02X\n", addr, value);
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_bios(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.bios[addr];
    log_spew("mmu_rd_bios: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_xrom(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xrom[addr & XROM_MASK];
    log_spew("mmu_rd_xrom: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_xrom_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xrom_bank[addr & XROM_MASK];
    log_spew("mmu_rd_xrom_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_xram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.xram_bank[addr & XRAM_MASK];
    log_spew("mmu_rd_xram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
void mmu_wr_xram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_xram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.xram_bank[addr & XRAM_MASK] = value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_vram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.vram_bank[addr & VRAM_MASK];
    log_spew("mmu_rd_vram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
void mmu_wr_vram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_vram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.vram_bank[addr & VRAM_MASK] = value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_wram(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.wram[addr & WRAM_MASK];
    log_spew("mmu_rd_wram: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
void mmu_wr_wram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_wram: addr=%04X value=%02X\n", addr, value);
    ctx->mem.wram[addr & WRAM_MASK] = value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_wram_bank(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = ctx->mem.wram_bank[addr & WRAM_MASK];
    log_spew("mmu_rd_wram_bank: addr=%04X value=%02X\n", addr, value);
    return value;
}

// -----------------------------------------------------------------------------
void mmu_wr_wram_bank(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("mmu_wr_wram_bank: addr=%04X value=%02X\n", addr, value);
    ctx->mem.wram_bank[addr & WRAM_MASK] = value;
}

// -----------------------------------------------------------------------------
uint8_t mmu_rd_oam(gbx_context_t *ctx, uint16_t addr)
{
#ifdef PROTECT_OAM_ACCESS
    if (!is_oam_accessible(ctx)) {
        log_err("attempted to read OAM when in use by LCD controller\n");
        return 0xFF;
    }
#endif

    log_spew("mmu_rd_oam: addr=%04X\n", addr);
    return ctx->mem.oam[addr & 0xFF];
}

// -----------------------------------------------------------------------------
void mmu_wr_oam(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
#ifdef PROTECT_OAM_ACCESS
    if (!is_oam_accessible(ctx)) {
        log_err("attempted to write OAM when in use by LCD controller\n");
        return;
    }
#endif

    log_spew("mmu_wr_oam: addr=%04X value=%02X\n", addr, value);
    ctx->mem.oam[addr & 0xFF] = value;
}

// -----------------------------------------------------------------------------
void cgb_set_vram_bank(gbx_context_t *ctx, uint8_t value)
{
    int bank = value & 1;
    if (!ctx->cgb_enabled) {
        log_err("cannot set VRAM bank when not in CGB mode\n");
        return;
    }

    ctx->mem.vram_bank = ctx->mem.vram + bank * VRAM_BANK_SIZE;
    ctx->mem.vram_bnum = bank;
    log_dbg("CGB set VRAM bank %02X (set bits %02X)\n", bank,  value);
}

// -----------------------------------------------------------------------------
void cgb_set_wram_bank(gbx_context_t *ctx, uint8_t value)
{
    int bank = (value & 3) ? (value & 3) : 1;
    if (!ctx->cgb_enabled) {
        log_err("cannot set WRAM bank when not in CGB mode\n");
        return;
    }

    ctx->mem.wram_bank = ctx->mem.wram + bank * WRAM_BANK_SIZE;
    ctx->mem.wram_bnum = bank;
    log_dbg("CGB set WRAM bank %02X (set bits %02X)\n", bank,  value);
}

// -----------------------------------------------------------------------------
INLINE void write_monochrome_palette(uint32_t *dest, uint8_t value)
{
    dest[0] = gbx_monochrome_colors[(value >> 0) & 3];
    dest[1] = gbx_monochrome_colors[(value >> 2) & 3];
    dest[2] = gbx_monochrome_colors[(value >> 4) & 3];
    dest[3] = gbx_monochrome_colors[(value >> 6) & 3];
}

// -----------------------------------------------------------------------------
INLINE uint32_t cgb_color_to_rgb(uint16_t c)
{
    return ((c & 0x001F) << 3) | ((c & 0x03E0) << 6) | ((c & 0x7C00) << 9);
}

// -----------------------------------------------------------------------------
INLINE void write_bcpd_color_palette(gbx_context_t *ctx, uint8_t value)
{
    int color, index = ctx->video.bcps & 0x3F;
    if (!ctx->cgb_enabled) {
        log_err("cannot write BG color palette when not in CGB mode\n");
        return;
    }

    ctx->video.bcpd[index] = value;
    color = ctx->video.bcpd[index & ~1] | (ctx->video.bcpd[index | 1] << 8);
    ctx->video.bcpd_rgb[index >> 1] = cgb_color_to_rgb(color);

    // if flag is set, auto increment the palette specification index by one
    if (ctx->video.bcps & 0x80)
        ctx->video.bcps = 0x80 | ((index + 1) & 0x3F);
}

// -----------------------------------------------------------------------------
INLINE void write_ocpd_color_palette(gbx_context_t *ctx, uint8_t value)
{
    int index = ctx->video.ocps & 0x3F;
    uint16_t color;
    if (!ctx->cgb_enabled) {
        log_err("cannot write OBJ color palette when not in CGB mode\n");
        return;
    }

    // store the color in both 16-bit CGB format and 32-bit ARGB
    ctx->video.ocpd[index] = value;
    color = ctx->video.ocpd[index & ~1] | (ctx->video.ocpd[index | 1] << 8);
    ctx->video.ocpd_rgb[index >> 1] = cgb_color_to_rgb(color);

    // if flag is set, auto increment the palette specification index by one
    if (ctx->video.ocps & 0x80)
        ctx->video.ocps = 0x80 | ((index + 1) & 0x3F);
}

// -----------------------------------------------------------------------------
INLINE uint8_t read_joyp_port(gbx_context_t *ctx)
{
    uint8_t input_bits = 0x0F;
    if (!(ctx->joyp & JOYP_SEL_DIR))
        input_bits = ctx->input_state & 0xF;
    else if (!(ctx->joyp & JOYP_SEL_BTN))
        input_bits = ctx->input_state >> 4;
    return ctx->joyp | input_bits | 0xC0;
}

// -----------------------------------------------------------------------------
INLINE uint8_t read_video_stat(gbx_context_t *ctx)
{
    int stat = ctx->video.stat & ~STAT_LYC;

    if (!(ctx->video.lcdc & LCDC_LCD_EN)) // XXX
        return 0x80;

    if (ctx->video.lcd_y == ctx->video.lyc)
        stat |= STAT_LYC;

    return stat | 0x80;
}

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
INLINE void write_lcdc(gbx_context_t *ctx, uint8_t value)
{
    // if the LCD enable/disable state changes, inform the frontend
    if ((ctx->video.lcdc ^ value) & LCDC_LCD_EN) {
        ext_lcd_enabled(ctx->userdata, (value & LCDC_LCD_EN) ? 1 : 0);
        if (!(value & LCDC_LCD_EN)) {
            ctx->video.lcd_x = 0;
            ctx->video.lcd_y = 0;
            ctx->video.state = VIDEO_STATE_SEARCH;
            ctx->video.cycle = 0;
        }
    }
    ctx->video.lcdc = value;
}

// -----------------------------------------------------------------------------
INLINE void start_dma_transfer(gbx_context_t *ctx, uint8_t value)
{
    if (ctx->dma.active) {
        log_err("attempting to start new DMA transfer with DMA active\n");
    }

    if (value > 0xF1) {
        log_err("specified DMA source address above F19F (%02X00)\n", value);
        value = 0xF1;
    }

    ctx->dma.src = value << 8;
    ctx->dma.active = 1;
    ctx->dma.cycle = 0;
    ctx->dma.write_pos = 0;
    ctx->dma.write_cycle = 0;
}

// -----------------------------------------------------------------------------
/*static*/ void hdma_hblank_transfer(gbx_context_t *ctx)
{
    log_spew("begin %d byte hblank dma transfer from %04X to %04X\n",
             ctx->video.hdma_len, ctx->video.hdma_src, ctx->video.hdma_dst);

    ctx->video.hdma_pos = 0;
    ctx->video.hdma_active = 1;
}

// -----------------------------------------------------------------------------
/*static*/ void hdma_general_purpose_transfer(gbx_context_t *ctx)
{
    int i;
    log_spew("begin %d byte general dma transfer from %04X to %04X\n",
             ctx->video.hdma_len, ctx->video.hdma_src, ctx->video.hdma_dst);

    for (i = 0; i < ctx->video.hdma_len; i++) {
        uint8_t data = gbx_read_byte(ctx, ctx->video.hdma_src + i);
        gbx_write_byte(ctx, ctx->video.hdma_dst + i, data);
    }

    ctx->video.hdma_ctl = 0xFF;
    ctx->video.hdma_active = 0;
}

#define IR_WR_DATA      0x01    // write data - enable or disable LED
#define IR_RD_DATA      0x02    // read data - 0 if receiving, 1 if normal
#define IR_RD_ENABLE    0xC0    // data read enable - 00 disabled, 11 enabled

// -----------------------------------------------------------------------------
/*static*/ void write_infrared_comm_port(gbx_context_t *ctx, uint8_t value)
{
    log_dbg("write %02X to IR comm port\n", value);
    ctx->rp = value & (IR_WR_DATA | IR_RD_ENABLE);
}

// -----------------------------------------------------------------------------
/*static*/ uint8_t read_infrared_comm_port(gbx_context_t *ctx)
{
    log_dbg("read %02X from IR comm port\n", 0);
    return ctx->rp | IR_RD_DATA;
}

// -----------------------------------------------------------------------------
/*static*/ uint8_t mmu_rd_himem(gbx_context_t *ctx, uint16_t addr)
{
    uint8_t value = 0;
    switch (addr & 0xFF) {
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
        value = ctx->int_en | ~INT_MASK; // set unused bits
        break;
    case PORT_LCDC:
        value = ctx->video.lcdc;
        break;
    case PORT_STAT:
        value = read_video_stat(ctx);
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
    case PORT_DMA:
        log_err("attempting to read write-only DMA port\n");
        value = ctx->dma.src; // ???
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
        value = ctx->video.wx;
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
        if (ctx->video.hdma_active)
            value = 0x80 | ((ctx->video.hdma_len >> 4) - 1);
        else
            value = ctx->video.hdma_ctl;
        break;
    case PORT_RP:
        value = read_infrared_comm_port(ctx);
        break;
    case PORT_BCPS:
        return ctx->video.bcps;
    case PORT_BCPD:
        return ctx->video.bcpd[ctx->video.bcps & 0x3F];
    case PORT_OCPS:
        return ctx->video.ocps;
    case PORT_OCPD:
        return ctx->video.ocpd[ctx->video.ocps & 0x3F];
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
        // TODO: implement
        value = ctx->mem.hram[addr & 0xFF];
        break;
    case PORT_BIOS:
        value = ctx->bios_enabled;
        break;
    default:
        value = ctx->mem.hram[addr & 0xFF];
        break;
    }

//  log_spew("read_io_port: port=%s addr=%04X value=%02X\n",
//           gbx_port_names[addr & 0xFF], addr, value);
    return value;
}

// -----------------------------------------------------------------------------
/*static*/ void mmu_wr_himem(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    uint16_t dma_addr;
//  log_spew("mmu_wr_himem: port=%s addr=%04X value=%02X\n",
//           gbx_port_names[addr & 0xFF], addr, value);

    switch (addr & 0xFF) {
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
        break;
    case PORT_TIMA:
        ctx->timer.tima = value;
        break;
    case PORT_TMA:
        ctx->timer.tma = value;
        break;
    case PORT_TAC:
        write_timer_control(ctx, value);
        break;
    case PORT_IF:
        ctx->int_flags = value & INT_MASK;
        break;
    case PORT_IE:
        ctx->int_en = value & INT_MASK;
        break;
    case PORT_LCDC:
        write_lcdc(ctx, value);
        break;
    case PORT_STAT:
        ctx->video.stat = value;
        break;
    case PORT_SCY:
        ctx->video.scy = value;
        break;
    case PORT_SCX:
        ctx->video.scx = value;
        break;
    case PORT_LY:
        log_err("attempting to write read-only PORT_LY (%02X)\n", value);
        break;
    case PORT_LYC:
        ctx->video.lyc = value;
        break;
    case PORT_DMA:
        start_dma_transfer(ctx, value);
        break;
    case PORT_BGP:
        ctx->video.bgp = value;
        write_monochrome_palette(ctx->video.bgp_rgb, value);
        break;
    case PORT_OBP0:
        ctx->video.obp0 = value;
        write_monochrome_palette(ctx->video.obp0_rgb, value);
        break;
    case PORT_OBP1:
        ctx->video.obp1 = value;
        write_monochrome_palette(ctx->video.obp1_rgb, value);
        break;
    case PORT_WY:
        ctx->video.wy = value;
        break;
    case PORT_WX:
        ctx->video.wx = value;
        break;
    case PORT_KEY1:
        // only prep bit is writeable, mask out read-only / unused bits
        ctx->key1 = (ctx->key1 & ~KEY1_PREP) | (value & KEY1_PREP);
        break;
    case PORT_VBK:
        cgb_set_vram_bank(ctx, value);
        break;
    case PORT_HDMA1:
        dma_addr = (ctx->video.hdma_src & 0x00FF) | (value << 8);
        ctx->video.hdma_src = gbx_validate_hdma_src(dma_addr);
        break;
    case PORT_HDMA2:
        dma_addr = (ctx->video.hdma_src & 0xFF00) | value;
        ctx->video.hdma_src = gbx_validate_hdma_src(dma_addr);
        break;
    case PORT_HDMA3:
        dma_addr = (ctx->video.hdma_dst & 0x00FF) | (value << 8);
        ctx->video.hdma_dst = gbx_validate_hdma_dst(dma_addr);
        break;
    case PORT_HDMA4:
        dma_addr = (ctx->video.hdma_dst & 0xFF00) | value;
        ctx->video.hdma_dst = gbx_validate_hdma_dst(dma_addr);
        break;
    case PORT_HDMA5:
        ctx->video.hdma_ctl = value;
        ctx->video.hdma_len = ((value & HDMA_LENGTH) + 1) << 4;
        if (value & HDMA_TYPE)
            hdma_hblank_transfer(ctx);
        else
            hdma_general_purpose_transfer(ctx);
        break;
    case PORT_RP:
        write_infrared_comm_port(ctx, value);
        break;
    case PORT_BCPS:
        ctx->video.bcps = value & 0xBF;
        break;
    case PORT_BCPD:
        write_bcpd_color_palette(ctx, value);
        break;
    case PORT_OCPS:
        ctx->video.ocps = value & 0xBF;
        break;
    case PORT_OCPD:
        write_ocpd_color_palette(ctx, value);
        break;
    case PORT_SVBK:
        cgb_set_wram_bank(ctx, value);
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
        break;
    case PORT_NR41:
    case PORT_NR42:
    case PORT_NR43:
    case PORT_NR44:
    case PORT_NR50:
    case PORT_NR51:
    case PORT_NR52:
        // TODO: implement
        ctx->mem.hram[addr & 0xFF] = value;
        break;
    case PORT_BIOS:
        if (ctx->bios_enabled) {
            ctx->bios_enabled = 0;
            mmu_map_bios(ctx, BIOS_UNMAP);
        }
        break;
    default:
        ctx->mem.hram[addr & 0xFF] = value;
        return;
    }
}

// -----------------------------------------------------------------------------
void mmu_map_bios(gbx_context_t *ctx, int setting)
{
    mmu_rd_fn fn = (setting == BIOS_MAP) ? mmu_rd_bios : mmu_rd_xrom;

    if (ctx->cgb_enabled) {
        // for the CGB, the bios image is mapped to 0000-00FF and 0200-08FF
        mmu_map_ro(ctx, 0x00, 1, fn);
        mmu_map_ro(ctx, 0x02, 7, fn);
    }
    else {
        // for the GMB/GBP/SGB, the bios image is a single page in length
        mmu_map_ro(ctx, 0x00, 1, fn);
    }
}

// -----------------------------------------------------------------------------
void mmu_map_pages(gbx_context_t *ctx)
{
    // only set read handlers from XROM - write is either NOP or MBC specific
    mmu_map_ro(ctx, 0x00, 0x40, mmu_rd_xrom);
    mmu_map_ro(ctx, 0x40, 0x40, mmu_rd_xrom_bank);

    // set RW handlers for VRAM, which is always (potentially) bankable
    mmu_map_rw(ctx, 0x80, 0x20, mmu_rd_vram_bank, mmu_wr_vram_bank);

    // map the external RAM handlers only if RAM present, set to NOP otherwise
    if (ctx->mem.xram_banks)
        mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_xram_bank, mmu_wr_xram_bank);
    else
        mmu_map_rw(ctx, 0xA0, 0x20, mmu_rd_nop, mmu_wr_nop);

    // set RW handlers for WRAM, first region is fixed, second is bankable
    mmu_map_rw(ctx, 0xC0, 0x10, mmu_rd_wram, mmu_wr_wram);
    mmu_map_rw(ctx, 0xD0, 0x10, mmu_rd_wram_bank, mmu_wr_wram_bank);

    // C000-CFFF mirrored at E000-EFFF, D000-DDFF mirrored at F000-FDFF
    mmu_map_rw(ctx, 0xE0, 0x10, mmu_rd_wram, mmu_wr_wram);
    mmu_map_rw(ctx, 0xF0, 0x0E, mmu_rd_wram_bank, mmu_wr_wram_bank);

    // the rest of high memory is OAM, IO ports, HRAM, or unallocated
    mmu_map_rw(ctx, 0xFE, 1, mmu_rd_oam, mmu_wr_oam);
    mmu_map_rw(ctx, 0xFF, 1, mmu_rd_himem, mmu_wr_himem);

    switch (ctx->cart_features & CART_MBC) {
    default:
        // shouldn't ever get here... but fall through just in case
        assert(!"unknown memory bank controller in mmu_map_pages");
    case CART_MBC_ROM:
        mmu_map_wo(ctx, 0x00, 0x80, mmu_wr_nop);
        break;
    case CART_MBC_MBC1: mmu_map_mbc1(ctx); break;
    case CART_MBC_MBC2: mmu_map_mbc2(ctx); break;
    case CART_MBC_MBC3: mmu_map_mbc3(ctx); break;
    case CART_MBC_MBC5: mmu_map_mbc5(ctx); break;
    }

    // if a bios ROM image is available, patch it into the memory map
    if (ctx->bios_enabled)
        mmu_map_bios(ctx, BIOS_MAP);
}

// -----------------------------------------------------------------------------
void mmu_map_ro(gbx_context_t *ctx, int beg, int n, mmu_rd_fn fn)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++)
        ctx->mem.page_rd[page] = fn;
}

// -----------------------------------------------------------------------------
void mmu_map_wo(gbx_context_t *ctx, int beg, int n, mmu_wr_fn fn)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++)
        ctx->mem.page_wr[page] = fn;
}

// -----------------------------------------------------------------------------
void mmu_map_rw(gbx_context_t *ctx, int beg, int n, mmu_rd_fn rf, mmu_wr_fn wf)
{
    int page, end = beg + n;
    for (page = beg; page < end; page++) {
        ctx->mem.page_rd[page] = rf;
        ctx->mem.page_wr[page] = wf;
    }
}

// -----------------------------------------------------------------------------
uint8_t gbx_read_byte(gbx_context_t *ctx, uint16_t addr)
{
    return ctx->mem.page_rd[addr >> 8](ctx, addr);
}

// -----------------------------------------------------------------------------
void gbx_write_byte(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    ctx->mem.page_wr[addr >> 8](ctx, addr, value);
}

