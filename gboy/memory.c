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
#include "ports.h"
#include "video.h"

// -----------------------------------------------------------------------------
static uint8_t read_bios_rom(gbx_context_t *ctx, uint16_t addr)
{
    switch (ctx->system) {
    case SYSTEM_GMB:
    case SYSTEM_SGB:
        if (addr < 0x100)
            return ctx->mem.bios[addr];
        break;
    case SYSTEM_CGB:
        if (addr < 0x100 || (addr >= 0x200 && addr < 0x900))
            return ctx->mem.bios[addr];
        break;
    }

    // otherwise, not a bios page, so read the XROM as we would normally
    return ctx->mem.xrom[addr & XROM_MASK];
}

// -----------------------------------------------------------------------------
static uint8_t read_bad_region(gbx_context_t *ctx, uint16_t addr)
{
    log_err("read_bad_region: addr=%04X\n", addr);
    return 0;
}

// -----------------------------------------------------------------------------
static void write_bad_region(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_err("write_bad_region: addr=%04X value=%02X\n", addr, value);
}

// -----------------------------------------------------------------------------
static uint8_t read_oam_region(gbx_context_t *ctx, uint16_t addr)
{
    log_spew("read_oam_region: addr=%04X\n", addr);
    return ctx->mem.oam[addr & 0xFF];
}

// -----------------------------------------------------------------------------
static void write_oam_region(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("write_oam_region: addr=%04X value=%02X\n", addr, value);
    ctx->mem.oam[addr & 0xFF] = value;
}

// -----------------------------------------------------------------------------
static uint8_t read_high_ram(gbx_context_t *ctx, uint16_t addr)
{
    log_spew("read_high_ram: addr=%04X\n", addr);
    return ctx->mem.hram[addr & 0xFF];
}

// -----------------------------------------------------------------------------
static void write_high_ram(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("write_high_ram: addr=%04X value=%02X\n", addr, value);
    ctx->mem.hram[addr & 0xFF] = value;
}

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
void mbc1_set_rom_bank(gbx_context_t *ctx, uint8_t value)
{
    if (ctx->mbc1_mode) {
        int bank = set_xrom_bank(ctx, (value & 0x1F) ? (value & 0x1F) : 1);
        log_dbg("MBC1 set XROM bank %02X (set bits %02X)\n", bank, value);
    }
    else {
        // cannot map bank 0 to programmable region (0x00, 0x20, 0x40, 0x60)
        int bank_lo = (value & 0x1F) ? (value & 0x1F) : 1;
        int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0xE0) | bank_lo);
        log_dbg("MBC1 set XROM bank %02X (set lo bits %02X)\n", bank, value);
    }
}

// -----------------------------------------------------------------------------
void mbc1_set_ram_bank(gbx_context_t *ctx, uint8_t value)
{
    if (ctx->mbc1_mode)  {
        // set the two bit RAM address, assuming XRAM is actually present
        if (ctx->mem.xram_banks) {
            int bank = set_xram_bank(ctx, value & 3);
            log_dbg("MBC1 set XRAM bank %02X (set bits %02X)\n", bank, value);
        }
        else
            log_err("MBC1 set XRAM bank %02X, but no XRAM present\n", value);
    }
    else {
        // set bits 5 and 6 of the XROM bank index when in mode 00
        int bank_hi = (value & 3) << 5;
        int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0x1F) | bank_hi);
        assert(ctx->mem.xrom_bnum & 0x1F);
        log_dbg("MBC1 set XROM bank %02X (set hi bits %02X)\n", bank, value);
    }
}

// -----------------------------------------------------------------------------
void mbc1_set_memory_model(gbx_context_t *ctx, uint8_t value)
{
    if (value & 1) {
        // RAM banking mode (4 Mb ROM / 32 KB RAM)
        ctx->mbc1_mode = 1;
    }
    else {
        // ROM banking mode (16 Mb ROM / 8 KB RAM)
        ctx->mbc1_mode = 0;
    }
    log_dbg("MBC1 set memory model to %d\n", ctx->mbc1_mode);
}

// -----------------------------------------------------------------------------
void mbc2_set_rom_bank(gbx_context_t *ctx, uint8_t value)
{
    // specify 4-bit ROM bank index, but 0 always maps to 1
    int bank = set_xrom_bank(ctx, (value & 0x0F) ? (value & 0x0F) : 1);
    log_dbg("MBC2 set XROM bank %02X (set bits %02X)\n", bank, value);
}

// -----------------------------------------------------------------------------
void mbc3_set_rom_bank(gbx_context_t *ctx, uint8_t value)
{
    // specify a full 7-bit ROM bank index, but 0 always maps to 1
    int bank = set_xrom_bank(ctx, (value & 0x7F) ? (value & 0x7F) : 1);
    log_dbg("MBC3 set XROM bank %02X (set bits %02X)\n", bank, value);
}

// -----------------------------------------------------------------------------
void mbc3_set_ram_bank(gbx_context_t *ctx, uint8_t value)
{
    if (value <= 0x03) {
        if (ctx->mem.xram_banks) {
            int bank = set_xram_bank(ctx, value & 0x03);
            log_dbg("MBC3 set XRAM bank %02X (set bits %02X)\n", bank,  value);
        }
        else
            log_err("MBC3 set XRAM bank %02X, but no XRAM present\n", value);
    }
    else {
        log_err("TODO: implement RTC register select\n");
    }
}

// -----------------------------------------------------------------------------
void mbc3_latch_clock_data(gbx_context_t *ctx, uint8_t value)
{
    log_err("TODO: implement latch clock data\n");
}

// -----------------------------------------------------------------------------
void mbc5_set_rom_bank_lo(gbx_context_t *ctx, uint8_t value)
{
    // specify a full 8-bit ROM bank index, but 0 always maps to 1
    int bank_lo = value ? value : 1;
    int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0x100) | bank_lo);
    log_dbg("MBC5 set XROM bank %03X (set lo bits %02X)\n", bank, value);
}

// -----------------------------------------------------------------------------
void mbc5_set_rom_bank_hi(gbx_context_t *ctx, uint8_t value)
{
    int bank_hi = (value & 1) << 8;
    int bank = set_xrom_bank(ctx, (ctx->mem.xrom_bnum & 0xFF) | bank_hi);
    log_dbg("MBC5 set XROM bank %03X (set hi bits %02X)\n", bank, value);
}

// -----------------------------------------------------------------------------
void mbc5_set_ram_bank(gbx_context_t *ctx, uint8_t value)
{
    if (ctx->mem.xram_banks) {
        int bank = set_xram_bank(ctx, value & 0x0F);
        log_dbg("MBC5 set XRAM bank %02X (set bits %02X)\n", bank,  value);
    }
    else
        log_err("MBC5 set XRAM bank %02X, but no XRAM present\n", value);
}

// -----------------------------------------------------------------------------
void cgb_set_vram_bank(gbx_context_t *ctx, uint8_t value)
{
    int bank = value & 1;
    if (!ctx->cgb_enabled) {
        log_err("cannot system VRAM bank when not in CGB mode\n");
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
        log_err("cannot system WRAM bank when not in CGB mode\n");
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
static uint8_t read_io_port(gbx_context_t *ctx, uint16_t addr)
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
    case PORT_HDMA2:
    case PORT_HDMA3:
    case PORT_HDMA4:
    case PORT_HDMA5:
        // TODO: implement
        value = ctx->mem.hram[addr & 0xFF];
        break;
    case PORT_RP:
        log_info("read RP = %02x\n", 0);
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
        log_err("attempting to read invalid io port (%04X)\n", addr);
        break;
    }

    log_spew("read_io_port: port=%s addr=%04X value=%02X\n",
             gbx_port_names[addr & 0xFF], addr, value);
    return value;
}

// -----------------------------------------------------------------------------
static void write_io_port(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    log_spew("write_io_port: port=%s addr=%04X value=%02X\n",
             gbx_port_names[addr & 0xFF], addr, value);

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
    case PORT_HDMA2:
    case PORT_HDMA3:
    case PORT_HDMA4:
    case PORT_HDMA5:
        // TODO: implement
        ctx->mem.hram[addr & 0xFF] = value;
        break;
    case PORT_RP:
        log_info("write RP = %02X\n", value);
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
        ctx->bios_enabled = 0;
        break;
    default:
        log_err("write to invalid io port (%04X) <- %02X\n", addr, value);
        return;
    }
}

// -----------------------------------------------------------------------------
uint8_t gbx_read_byte(gbx_context_t *ctx, uint16_t addr)
{
    switch (addr >> 12) {
    case 0x0: case 0x1: case 0x2: case 0x3:
        if (ctx->bios_enabled)
            return read_bios_rom(ctx, addr);
        else
            return ctx->mem.xrom[addr & XROM_MASK];
    case 0x4: case 0x5: case 0x6: case 0x7:
        return ctx->mem.xrom_bank[addr & XROM_MASK];
    case 0x8: case 0x9:
        return ctx->mem.vram_bank[addr & VRAM_MASK];
    case 0xA: case 0xB:
        if (ctx->have_mbc2)
            return ctx->mem.xram[addr & 0x1FF];
        else if (!ctx->mem.xram_bank)
            log_err("bad XRAM read - addr:%04X\n", addr);
        else
            return ctx->mem.xram_bank[addr & XRAM_MASK];
        break;
    case 0xC:
        return ctx->mem.wram[addr & WRAM_MASK];
    case 0xD:
        return ctx->mem.wram_bank[addr & WRAM_MASK];
    case 0xE:
        return ctx->mem.wram[addr & WRAM_MASK];
    case 0xF:
        if (addr < 0xFE00) return ctx->mem.wram_bank[addr & WRAM_MASK];
        if (addr < 0xFEA0) return read_oam_region(ctx, addr);
        if (addr < 0xFF00) return read_bad_region(ctx, addr);
        if (addr < 0xFF80) return read_io_port(ctx, addr);
        if (addr < 0xFFFF) return read_high_ram(ctx, addr);
        else return read_io_port(ctx, addr);
    }

    log_err("gbx_read_byte: invalid address (%04X)\n", addr);
    return 0;
}

// -----------------------------------------------------------------------------
void gbx_write_byte(gbx_context_t *ctx, uint16_t addr, uint8_t value)
{
    memory_regions_t *mem = &ctx->mem;

    switch (addr >> 12) {
    case 0x0: case 0x1:
        if (ctx->have_mbc1 || ctx->have_mbc2 || ctx->have_mbc3)
            log_dbg("enable RAM access: %02X\n", value);
        else
            log_err("bad XROM write - addr:%04X val:%02X\n", addr, value);
        break;
    case 0x2:
        if (ctx->have_mbc5) {
            mbc5_set_rom_bank_lo(ctx, value);
            break;
        }
        // fall through for other chips
    case 0x3:
        if (ctx->have_mbc1) 
            mbc1_set_rom_bank(ctx, value);
        else if (ctx->have_mbc2)
            mbc2_set_rom_bank(ctx, value);
        else if (ctx->have_mbc3)
            mbc3_set_rom_bank(ctx, value);
        else if (ctx->have_mbc5)
            mbc5_set_rom_bank_hi(ctx, value);
        else
            log_err("bad XROM write - addr:%04X val:%02X\n", addr, value);
        break;
    case 0x4: case 0x5:
        if (ctx->have_mbc1)
            mbc1_set_ram_bank(ctx, value);
        else if (ctx->have_mbc3)
            mbc3_set_ram_bank(ctx, value);
        else if (ctx->have_mbc5)
            mbc5_set_ram_bank(ctx, value);
        else
            log_err("bad XROM write - addr:%04X val:%02X\n", addr, value);
        break;
    case 0x6: case 0x7:
        if (ctx->have_mbc1)
            mbc1_set_memory_model(ctx, value);
        else if (ctx->have_mbc3)
            mbc3_latch_clock_data(ctx, value);
        else
            log_err("bad XROM write - addr:%04X val:%02X\n", addr, value);
        break;
    case 0x8: case 0x9:
        mem->vram_bank[addr & VRAM_MASK] = value;
        break;
    case 0xA: case 0xB:
        if (ctx->have_mbc2)
            ctx->mem.xram[addr & 0x1FF] = value & 0x0F;
        else if (!mem->xram_bank)
            log_err("bad XRAM write - addr:%04X val:%02X\n", addr, value);
        else
            mem->xram_bank[addr & XRAM_MASK] = value;
        break;
    case 0xC:
        mem->wram[addr & WRAM_MASK] = value;
        break;
    case 0xD:
        mem->wram_bank[addr & WRAM_MASK] = value;
        break;
    case 0xE:
        mem->wram[addr & WRAM_MASK] = value;
        break;
    case 0xF:
        if (addr < 0xFE00)
            mem->wram_bank[addr & WRAM_MASK] = value;
        else if (addr < 0xFEA0)
            write_oam_region(ctx, addr, value);
        else if (addr < 0xFF00)
            write_bad_region(ctx, addr, value);
        else if (addr < 0xFF80)
            write_io_port(ctx, addr, value);
        else if (addr < 0xFFFF)
            write_high_ram(ctx, addr, value);
        else
            write_io_port(ctx, addr, value);
        break;
    }
}

