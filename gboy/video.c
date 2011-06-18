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
#include "ports.h"
#include "video.h"

// -----------------------------------------------------------------------------
INLINE void sprite_normal(gbx_context_t *ctx, uint32_t *palette,
        uint8_t *tile, int xpos, int ypos, int w0, int h0, int w1, int h1)
{
    int x, y, w, h, val, a, b;
    for (h = h0; h <= h1; ++h) {
        a = *tile++;
        b = *tile++;
        for (w = w0; w <= w1; ++w) {
            x = xpos + w;
            y = ypos + h;
            val = ((a >> (7 - w)) & 1) | (((b >> (7 - w)) << 1) & 2);
            if (val) ctx->fb[y * GBX_LCD_XRES + x] = palette[val];
        }
    }
}

// -----------------------------------------------------------------------------
INLINE void sprite_xflip(gbx_context_t *ctx, uint32_t *palette,
        uint8_t *tile, int xpos, int ypos, int w0, int h0, int w1, int h1)
{
    int x, y, w, h, val, a, b;
    for (h = h0; h <= h1; ++h) {
        a = *tile++;
        b = *tile++;
        for (w = w0; w <= w1; ++w) {
            x = xpos + w;
            y = ypos + h;
            val = ((a >> w) & 1) | (((b >> w) << 1) & 2);
            if (val) ctx->fb[y * GBX_LCD_XRES + x] = palette[val];
        }
    }
}

// -----------------------------------------------------------------------------
INLINE void sprite_yflip(gbx_context_t *ctx, uint32_t *palette,
        uint8_t *tile, int xpos, int ypos, int w0, int h0, int w1, int h1)
{
    int x, y, w, h, val, a, b;
    for (h = h0; h <= h1; ++h) {
        b = *tile--;
        a = *tile--;
        for (w = w0; w <= w1; ++w) {
            x = xpos + w;
            y = ypos + h;
            val = ((a >> (7 - w)) & 1) | (((b >> (7 - w)) << 1) & 2);
            if (val) ctx->fb[y * GBX_LCD_XRES + x] = palette[val];
        }
    }
}

// -----------------------------------------------------------------------------
INLINE void sprite_xyflip(gbx_context_t *ctx, uint32_t *palette,
        uint8_t *tile, int xpos, int ypos, int w0, int h0, int w1, int h1)
{
    int x, y, w, h, val, a, b;
    for (h = h0; h <= h1; ++h) {
        b = *tile--;
        a = *tile--;
        for (w = w0; w <= w1; ++w) {
            x = xpos + w;
            y = ypos + h;
            val = ((a >> w) & 1) | (((b >> w) << 1) & 2);
            if (val) ctx->fb[y * GBX_LCD_XRES + x] = palette[val];
        }
    }
}

// -----------------------------------------------------------------------------
void render_sprites(gbx_context_t *ctx)
{
    int i, xpos, ypos, w0, h0, w1, h1, type;
    uint8_t pnum, attr, *tile;
    uint16_t oam_addr = 0xFE00;
    uint32_t *palette;

    // check for 8x8 or 8x16 sprite, for 8x16 pattern LSB is ignored
    int SH = (ctx->video.lcdc & LCDC_OBJ_SIZE) ? 16 : 8;
    uint8_t pnum_mask = (ctx->video.lcdc & LCDC_OBJ_SIZE) ? 0xFE : 0xFF;

    for (i = 0; i < 40; ++i) {
        ypos = gbx_read_byte(ctx, oam_addr++) - 16;
        xpos = gbx_read_byte(ctx, oam_addr++) - 8;
        pnum = gbx_read_byte(ctx, oam_addr++) & pnum_mask;
        attr = gbx_read_byte(ctx, oam_addr++);

        if (ypos == -16 || ypos >= GBX_LCD_YRES ||
            xpos ==  -8 || xpos >= GBX_LCD_XRES)
            continue;

        // determine the orientation (normal, xflip, yflip, xflip & yflip)
        type = ((attr & OAM_ATTR_XFLIP) >> 5) | ((attr & OAM_ATTR_YFLIP) >> 5);

        if (ctx->cgb_enabled) {
            // select the palette base address (OBP0-7)
            palette = &ctx->video.ocpd_rgb[(attr & OAM_ATTR_CPAL) << 2];

            // select the tile data from either VRAM bank 0 or bank 1
            if (attr & OAM_ATTR_BANK)
                tile = &ctx->mem.vram[VRAM_BANK_SIZE + (pnum << 4)];
            else
                tile = &ctx->mem.vram[pnum << 4];
        }
        else {
            // monochrome mode: select from either OBP0 or OBP1
            if (attr & OAM_ATTR_PAL)
                palette = ctx->video.obp1_rgb;
            else
                palette = ctx->video.obp0_rgb;

            // tile data located in the first (and only) VRAM bank
            tile = &ctx->mem.vram[pnum << 4];
        }

        w0 = (xpos >= 0) ? 0 : -xpos;
        h0 = (ypos >= 0) ? 0 : -ypos;

        w1 = (xpos <= (GBX_LCD_XRES - 8)) ? 7 : (GBX_LCD_XRES - xpos - 1);
        h1 = (ypos <= (GBX_LCD_YRES - SH)) ? SH-1 : (GBX_LCD_YRES - ypos - 1);

        switch (type) {
        case 0:
            tile += (h0 << 1);
            sprite_normal(ctx, palette, tile, xpos, ypos, w0, h0, w1, h1);
            break;
        case 1:
            tile += (h0 << 1);
            sprite_xflip(ctx, palette, tile, xpos, ypos, w0, h0, w1, h1);
            break;
        case 2:
            tile += ((SH - 1 - h0) << 1) + 1;
            sprite_yflip(ctx, palette, tile, xpos, ypos, w0, h0, w1, h1);
            break;
        case 3:
            tile += ((SH - 1 - h0) << 1) + 1;
            sprite_xyflip(ctx, palette, tile, xpos, ypos, w0, h0, w1, h1);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
void render_bg_pixel(gbx_context_t *ctx, int x, int y)
{
    int base_x, base_y, off_x, off_y, mappos, cnum;
    int wx = ctx->video.wx - 7, wy = ctx->video.wy, mapaddr;
    uint8_t tile, c1, c2, attr;
    uint16_t addr, char_base = 0;
    uint32_t *palette = ctx->video.bgp_rgb;

    if ((ctx->video.lcdc & LCDC_WND_EN) && (x >= wx) && (y >= wy)) {
        base_x = x - wx;
        base_y = y - wy;
        mappos = ((base_y & 0xF8) << 2) | (base_x >> 3);

        // window tile index may be located at either 0x9800 or 0x9C00
        mapaddr = mappos + (ctx->video.lcdc & LCDC_WND_CODE ? 0x1C00 : 0x1800);
    }
    else {
        base_x = (x + ctx->video.scx) & 0xFF;
        base_y = (y + ctx->video.scy) & 0xFF;
        mappos = ((base_y & 0xF8) << 2) | (base_x >> 3);

        // background tile index may be located at either 0x9800 or 0x9C00
        mapaddr = mappos + (ctx->video.lcdc & LCDC_BG_CODE ? 0x1C00 : 0x1800);
    }

    off_x = 7 - (base_x & 7);
    off_y = base_y & 7;
    tile = ctx->mem.vram[mapaddr];

    if (ctx->cgb_enabled) {
        // read the background map attribute when operating in color mode
        attr = ctx->mem.vram[mapaddr + VRAM_BANK_SIZE];

        // determine whether the tile data is located in bank 0 or bank 1
        if (attr & BG_ATTR_BANK)
            char_base = VRAM_BANK_SIZE;

        // check if we need to flip the tile in either the x or y direction
        if (attr & BG_ATTR_XFLIP) off_x = 7 - off_x;
        if (attr & BG_ATTR_YFLIP) off_y = 7 - off_y;

        palette = &ctx->video.bcpd_rgb[(attr & BG_ATTR_PAL) << 2];
    }

    // tile character data may be located at either 0x8000 or 0x8800
    if (ctx->video.lcdc & LCDC_BG_CHAR)
        addr = char_base + (tile << 4) + (off_y << 1);
    else
        addr = char_base + 0x1000 + ((int8_t)tile << 4) + (off_y << 1);

    // compute the color and store it in the framebuffer
    c1 = ctx->mem.vram[addr + 0];
    c2 = ctx->mem.vram[addr + 1];
    cnum = ((c1 >> off_x) & 1) | (((c2 >> off_x) << 1) & 2);
    ctx->fb[y * GBX_LCD_XRES + x] = palette[cnum];
}

// -----------------------------------------------------------------------------
INLINE void set_stat_mode(gbx_context_t *ctx, int mode)
{
    assert(mode >= 0 && mode <= 3);
    ctx->video.stat = (ctx->video.stat & ~STAT_MODE) | mode;
    log_spew("LCD STAT mode changing to %d\n", mode);
}

// -----------------------------------------------------------------------------
INLINE void check_coincidence(gbx_context_t *ctx)
{
    // fire an interrupt when LY == LYC becomes true and interrupt enabled
    if (ctx->video.lcd_y == ctx->video.lyc && ctx->video.stat & STAT_INT_LYC)
        gbx_req_interrupt(ctx, INT_LCDSTAT);

    // this is only called when Y transitions, so rather than set the STAT LYC
    // flag here, it is computed on demand when the STAT register is read
}

// -----------------------------------------------------------------------------
INLINE void transition_to_search(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_SEARCH;
    ctx->video.cycle = 0;
    ctx->video.lcd_x = 0;

    // check for OAM STAT interrupt
    set_stat_mode(ctx, MODE_SEARCH);
    if (ctx->video.stat & STAT_INT_OAM)
        gbx_req_interrupt(ctx, INT_LCDSTAT);
}

// -----------------------------------------------------------------------------
INLINE void transition_to_transfer(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_TRANSFER;
    ctx->video.cycle = 0;

    // no SEARCH STAT interrupt
    set_stat_mode(ctx, MODE_TRANSFER);
}

// -----------------------------------------------------------------------------
static void hdma_transfer_block(gbx_context_t *ctx)
{
    uint16_t src = ctx->video.hdma_src + ctx->video.hdma_pos;
    uint16_t dst = ctx->video.hdma_dst + ctx->video.hdma_pos;
    int i, copy_length = MIN(0x10, ctx->video.hdma_len);

    for (i = 0; i < copy_length; i++) {
        uint8_t data = gbx_read_byte(ctx, src + i);
        gbx_write_byte(ctx, dst + i, data);
    }

    ctx->video.hdma_len -= copy_length;
    ctx->video.hdma_pos += copy_length;

    if (0 >= ctx->video.hdma_len)
        ctx->video.hdma_active = 0;

    log_spew("HDMA copied %02X bytes from %04X to %04X (%02X left)\n",
             copy_length, src, dst, ctx->video.hdma_len);
}

// -----------------------------------------------------------------------------
INLINE void transition_to_hblank(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_HBLANK;
    ctx->video.cycle = 0;

    // if there is an HBLANK DMA pending, perform a 16 byte transfer
    if (ctx->video.hdma_active)
        hdma_transfer_block(ctx);

    // check for HBLANK STAT interrupt
    set_stat_mode(ctx, MODE_HBLANK);
    if (ctx->video.stat & STAT_INT_HBLANK)
        gbx_req_interrupt(ctx, INT_LCDSTAT);
}

// -----------------------------------------------------------------------------
INLINE void transition_to_vblank(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_VBLANK;
    ctx->video.cycle = 0;

    render_sprites(ctx);
    ext_video_sync(ctx->userdata);
    gbx_req_interrupt(ctx, INT_VBLANK);

    // check for VBLANK STAT interrupt
    set_stat_mode(ctx, MODE_VBLANK);
    if (ctx->video.stat & STAT_INT_VBLANK)
        gbx_req_interrupt(ctx, INT_LCDSTAT);
}

// -----------------------------------------------------------------------------
void video_update_cycles(gbx_context_t *ctx, long cycles)
{
    long i;

    // if in double speed mode, halve the number of LCD clock cycles
    if (ctx->key1 & KEY1_SPEED)
        cycles >>= 1;

    if (!(ctx->video.lcdc & LCDC_LCD_EN))
        return;

    // drive the LCD for each cycle elapsed, inefficient but accurate
    for (i = 0; i < cycles; i++) {

        switch (ctx->video.state) {
        case VIDEO_STATE_SEARCH:
            // check for OAM search completion, transition to data transfer
            if (++ctx->video.cycle >= VIDEO_CYCLES_SEARCH)
                transition_to_transfer(ctx);
            break;
        case VIDEO_STATE_TRANSFER:
            // render each pixel of the current scanline
            if (ctx->video.lcd_x < GBX_LCD_XRES) {
                render_bg_pixel(ctx, ctx->video.lcd_x, ctx->video.lcd_y);
                ++ctx->video.lcd_x;
            }

            // check for data transfer completion, transition to h-blank
            if (++ctx->video.cycle >= VIDEO_CYCLES_TRANSFER)
                transition_to_hblank(ctx);
            break;
        case VIDEO_STATE_HBLANK:
            // check for h-blank completion, transition to v-blank or search
            if (++ctx->video.cycle >= VIDEO_CYCLES_HBLANK) {
                if (++ctx->video.lcd_y >= GBX_LCD_YRES)
                    transition_to_vblank(ctx);
                else
                    transition_to_search(ctx);

                // check for coincidence interrupt each time LY changes
                check_coincidence(ctx);
            }
            break;
        case VIDEO_STATE_VBLANK:
            // check for v-blank completion, transition to oam search
            if (++ctx->video.cycle >= VIDEO_CYCLES_SCANLINE) {
                if (++ctx->video.lcd_y >= LCD_SCANLINE_COUNT) {
                    transition_to_search(ctx);
                    ctx->video.lcd_y = 0;
                }

                // check for coincidence interrupt each time LY changes
                check_coincidence(ctx);
                ctx->video.cycle = 0;
            }
            break;
        }
    }
}

