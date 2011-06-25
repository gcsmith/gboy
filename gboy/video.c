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

#include <string.h>
#include <assert.h>
#include "gbx.h"
#include "memory.h"
#include "ports.h"
#include "video.h"

// ----------------------------------------------------------------------------
void video_write_mono_palette(uint32_t *dest, uint8_t value)
{
    static const uint32_t monochrome_colors[4] = {
        0xFFFFFFFF, 0x80808080, 0x40404040, 0x00000000
    };

    dest[0] = monochrome_colors[(value >> 0) & 3];
    dest[1] = monochrome_colors[(value >> 2) & 3];
    dest[2] = monochrome_colors[(value >> 4) & 3];
    dest[3] = monochrome_colors[(value >> 6) & 3];
}

// ----------------------------------------------------------------------------
void video_write_lcdc(gbx_context_t *ctx, uint8_t value)
{
    // if the LCD enable/disable state changes, inform the frontend
    if ((ctx->video.lcdc ^ value) & LCDC_LCD_EN) {
        ext_lcd_enabled(ctx->userdata, (value & LCDC_LCD_EN) ? 1 : 0);

        // force reset of video state machine when if LCD disabled
        if (!(value & LCDC_LCD_EN)) {
            ctx->video.lcd_x = 0;
            ctx->video.lcd_y = 0;
            ctx->video.state = VIDEO_STATE_SEARCH;
            ctx->video.cycle = 0;
        }
    }

    // determine if window tile map is located at 9C00 or 9800
    if (value & LCDC_WND_CODE)
        ctx->video.wnd_code = &ctx->mem.vram[0x1C00];
    else
        ctx->video.wnd_code = &ctx->mem.vram[0x1800];

    // determine if background tile map is located at 9C00 or 9800
    if (value & LCDC_BG_CODE)
        ctx->video.bg_code = &ctx->mem.vram[0x1C00];
    else
        ctx->video.bg_code = &ctx->mem.vram[0x1800];

    if (value & LCDC_OBJ_SIZE) {
        ctx->video.sprite_hmax = 15;
        ctx->video.sprite_mask = 0xFE;
    }
    else {
        ctx->video.sprite_hmax = 7;
        ctx->video.sprite_mask = 0xFF;
    }

    // set all layers to enabled and standard priority, then alter below...
    ctx->video.show_bg = ctx->video.show_wnd = ctx->video.show_obj = 1;
    ctx->video.obj_pri = 0;

    if (!(value & LCDC_OBJ_EN))
        ctx->video.show_obj = 0;

    if (!(value & LCDC_WND_EN))
        ctx->video.show_wnd = 0;

    // BG_EN is odd in that its behavior depends on the system type and whether
    // we're running in color or monochrome mode, and may override other bits

    if (!(value & LCDC_BG_EN)) {
        if (ctx->system == SYSTEM_CGB) {
            if (ctx->color_enabled) {
                // CGB in color mode -- set object priority over BG and window
                // keep unset if objects are disabled
                ctx->video.obj_pri = ctx->video.show_obj;
            }
            else {
                // CGB in monochrome mode -- disable BG and window layer
                // note that the WND_EN bit is overridden in this case
                ctx->video.show_bg = 0;
                ctx->video.show_wnd = 0;
            }
        }
        else {
            // non-CGB / monochrome system -- only disable the BG layer
            ctx->video.show_bg = 0;
        }
    }

    ctx->video.lcdc = value;
}

// ----------------------------------------------------------------------------
void video_write_bcpd(gbx_context_t *ctx, uint8_t value)
{
    int color, index = ctx->video.bcps & CPS_INDEX;
    if (!ctx->color_enabled) {
        log_warn("cannot write BG color palette when not in CGB mode\n");
        return;
    }

    ctx->video.bcpd[index] = value;
    color = ctx->video.bcpd[index & ~1] | (ctx->video.bcpd[index | 1] << 8);
    ctx->video.bcpd_rgb[index >> 1] = cgb_color_to_rgb(color);

    // if flag is set, auto increment the palette specification index by one
    if (ctx->video.bcps & CPS_INCREMENT)
        ctx->video.bcps = CPS_INCREMENT | ((index + 1) & CPS_INDEX);
}

// ----------------------------------------------------------------------------
void video_write_ocpd(gbx_context_t *ctx, uint8_t value)
{
    int index = ctx->video.ocps & CPS_INDEX;
    uint16_t color;
    if (!ctx->color_enabled) {
        log_warn("cannot write OBJ color palette when not in CGB mode\n");
        return;
    }

    // store the color in both 16-bit CGB format and 32-bit ARGB
    ctx->video.ocpd[index] = value;
    color = ctx->video.ocpd[index & ~1] | (ctx->video.ocpd[index | 1] << 8);
    ctx->video.ocpd_rgb[index >> 1] = cgb_color_to_rgb(color);

    // if flag is set, auto increment the palette specification index by one
    if (ctx->video.ocps & CPS_INCREMENT)
        ctx->video.ocps = CPS_INCREMENT | ((index + 1) & CPS_INDEX);
}

// ----------------------------------------------------------------------------
static void hdma_hblank_transfer_block(gbx_context_t *ctx)
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

    log_dbg("HDMA copied %02X bytes from %04X to %04X (%04X left)\n",
             copy_length, src, dst, ctx->video.hdma_len);

    ctx->cycle_delta += (copy_length >> 4) * (ctx->fast_mode ? 16 : 8);
}

// ----------------------------------------------------------------------------
static void video_schedule_hblank_dma(gbx_context_t *ctx, int length)
{
    ctx->video.hdma_pos = 0;
    ctx->video.hdma_len = length;
    ctx->video.hdma_active = 1;

    log_dbg("begin %04X byte h-blank DMA transfer from %04X to %04X\n",
             ctx->video.hdma_len, ctx->video.hdma_src, ctx->video.hdma_dst);
}

// ----------------------------------------------------------------------------
static void video_schedule_general_dma(gbx_context_t *ctx, int copy_length)
{
    int i;
    ctx->video.hdma_active = 0;
    ctx->video.hdma_len = copy_length;

    log_dbg("begin %04X byte general DMA transfer from %04X to %04X\n",
             ctx->video.hdma_len, ctx->video.hdma_src, ctx->video.hdma_dst);

    for (i = 0; i < copy_length; i++) {
        uint8_t data = gbx_read_byte(ctx, ctx->video.hdma_src + i);
        gbx_write_byte(ctx, ctx->video.hdma_dst + i, data);
    }

    ctx->cycle_delta += (copy_length >> 4) * (ctx->fast_mode ? 16 : 8);
}

// ----------------------------------------------------------------------------
void video_write_hdma(gbx_context_t *ctx, uint8_t value)
{
    // check for attempt to cancel HDMA (or erroneous attempt to schedule)
    if (ctx->video.hdma_active) {
        if (value & 0x80) {
            log_err("attempt to schedule HDMA while currently active\n");
            return;
        }
        else {
            log_dbg("cancelling current HDMA operation w/ %d bytes left\n",
                    ctx->video.hdma_len);
            ctx->video.hdma_active = 0;
            return;
        }
    }

    if (value & HDMA_TYPE)
        video_schedule_hblank_dma(ctx, ((value & HDMA_LENGTH) + 1) << 4);
    else
        video_schedule_general_dma(ctx, ((value & HDMA_LENGTH) + 1) << 4);
}

// ----------------------------------------------------------------------------
uint8_t video_read_hdma(gbx_context_t *ctx)
{
    if (ctx->video.hdma_active)
        return ((ctx->video.hdma_len >> 4) - 1);
    else
        return 0xFF;
}

// ----------------------------------------------------------------------------
void video_write_stat(gbx_context_t *ctx, uint8_t value)
{
    ctx->video.stat = value & ~(STAT_MODE | STAT_LYC);
}

// ----------------------------------------------------------------------------
uint8_t video_read_stat(gbx_context_t *ctx)
{
    int stat = ctx->video.stat & ~STAT_LYC;

    if (!(ctx->video.lcdc & LCDC_LCD_EN)) // XXX
        return 0x80;

    if (ctx->video.lcd_y == ctx->video.lyc)
        stat |= STAT_LYC;

    return stat | 0x80;
}

// ----------------------------------------------------------------------------
INLINE void commit_sprite_color(gbx_context_t *ctx, int sprite, int x, int y)
{
    obj_char_t *obj = &((obj_char_t *)ctx->mem.oam)[sprite];
    uint32_t *palette = ctx->video.bgp_rgb;
    int ci, c1, c2, addr;
    
    int code = obj->code & ctx->video.sprite_mask;
    int off_x = x - obj->xpos + 8;
    int off_y = y - obj->ypos + 16;

    // determine the orientation (normal, xflip, yflip, xflip & yflip)
    if (!(obj->attr & OAM_ATTR_XFLIP)) off_x = 7 - off_x;
    if (obj->attr & OAM_ATTR_YFLIP) off_y = ctx->video.sprite_hmax - off_y;

    if (ctx->color_enabled) {
        // select the palette base address (OBP0-7)
        palette = &ctx->video.ocpd_rgb[(obj->attr & OAM_ATTR_CPAL) << 2];

        // select the tile data from either VRAM bank 0 or bank 1
        if (obj->attr & OAM_ATTR_BANK)
            addr = VRAM_BANK_SIZE + (code << 4) + (off_y << 1);
        else
            addr = (code << 4) + (off_y << 1);
    }
    else {
        // monochrome mode: select from either OBP0 or OBP1
        if (obj->attr & OAM_ATTR_PAL)
            palette = ctx->video.obp1_rgb;
        else
            palette = ctx->video.obp0_rgb;

        // tile data located in the first (and only) VRAM bank
        addr = (code << 4) + (off_y << 1);
    }

    c1 = ctx->mem.vram[addr + 0];
    c2 = ctx->mem.vram[addr + 1];
    ci = ((c1 >> off_x) & 1) | (((c2 >> off_x) << 1) & 2);

    if (ci) {
        ctx->video.line_obj[x] = sprite;
        ctx->video.line_col[x] = palette[ci];
    }
}

// ----------------------------------------------------------------------------
static void prepare_line_buffer(gbx_context_t *ctx)
{
    obj_char_t *obj = (obj_char_t *)ctx->mem.oam;
    int *line = ctx->video.line_obj;
    int height = (ctx->video.lcdc & LCDC_OBJ_SIZE) ? 16 : 8;
    int old, xpos, ypos, x, sprite, curr_line = ctx->video.lcd_y;

    // clear the object line buffer, any index < 0 considered uninitialized
    memset(line, 0xFF, sizeof(int) * GBX_LCD_XRES);

    for (sprite = 0; sprite < 40; ++sprite) {
        // skip this sprite if it doesn't touch any pixels on the current line
        ypos = obj[sprite].ypos - 16;
        if (curr_line < ypos || curr_line >= (ypos + height))
            continue;
        
        xpos = obj[sprite].xpos - 8;
        for (x = xpos; x < xpos + 8; x++) {
            // obviously skip if the current sprite pixel is not on the screen
            if (x < 0 || x >= GBX_LCD_XRES)
                continue;

            old = line[x];
            if (old >= 0) {
                // lowest OBJ index always taken on CGB regardless of X coord
                if (obj[old].xpos == obj[sprite].xpos || ctx->color_enabled)
                    continue;

                // for DMG (or DMG mode), however, take the lowest x coord
                if (obj[old].xpos < obj[sprite].xpos)
                    continue;
            }

            // only assign the sprite to this column if its not transparent
            commit_sprite_color(ctx, sprite, x, curr_line);
        }
    }
}

// ----------------------------------------------------------------------------
static void video_render_pixel(gbx_context_t *ctx, int x, int y)
{
    const int fb_pos = y * GBX_LCD_XRES + x;
    int base_x, base_y, off_x, off_y, ci, bg_max_pri = 0;
    uint8_t *ptile, *pcolor = ctx->mem.vram;
    uint32_t *palette = ctx->video.bgp_rgb;
    int sprite = ctx->video.line_obj[x];

    if (sprite >= 0) {
        obj_char_t *obj = &((obj_char_t *)ctx->mem.oam)[sprite];
        if (ctx->video.obj_pri) {
            // always render sprite if present and max priority given in LCDC
            ctx->fb[fb_pos] = ctx->video.line_col[x];
            return;
        }
        else if (obj->attr & OAM_ATTR_PRI) {
            // even if not set here, OBJ priority may be overriden by BG attr
            bg_max_pri = 1;
        }
    }

    // if obj_pri is NOT set, need to evalulate both BG and OBJ priority
    if (ctx->video.show_wnd && x >= ctx->video.wx && y >= ctx->video.wy) {
        base_x = x - ctx->video.wx;
        base_y = ctx->video.curr_wy;
        if (x == GBX_LCD_XRES-1) ctx->video.curr_wy++;
        ptile = &ctx->video.wnd_code[((base_y & 0xF8) << 2) | (base_x >> 3)];
    }
    else if (ctx->video.show_bg) {
        base_x = (x + ctx->video.scx) & 0xFF;
        base_y = (y + ctx->video.scy) & 0xFF;
        ptile = &ctx->video.bg_code[((base_y & 0xF8) << 2) | (base_x >> 3)];
    }
    else {
        ctx->fb[fb_pos] = sprite > 0 ? ctx->video.line_col[x] : 0;
        return;
    }

    off_x = 7 - (base_x & 7);
    off_y = base_y & 7;

    if (ctx->color_enabled) {
        // read the background map attribute when operating in color mode
        uint8_t attr = *(ptile + VRAM_BANK_SIZE);

        // determine whether the tile data is located in bank 0 or bank 1
        if (attr & BG_ATTR_BANK)
            pcolor += VRAM_BANK_SIZE;

        if (attr & BG_ATTR_PRI)
            bg_max_pri = 1;

        // check if we need to flip the tile in either the x or y direction
        if (attr & BG_ATTR_XFLIP) off_x = 7 - off_x;
        if (attr & BG_ATTR_YFLIP) off_y = 7 - off_y;

        palette = &ctx->video.bcpd_rgb[(attr & BG_ATTR_PAL) << 2];
    }

    // tile character data may be indexed from either 0x8000 or 0x8800
    if (ctx->video.lcdc & LCDC_BG_CHAR)
        pcolor += (*ptile << 4) + (off_y << 1);
    else
        pcolor += 0x1000 + ((int8_t)*ptile << 4) + (off_y << 1);

    // compute the color and store it in the framebuffer
    ci = ((pcolor[0] >> off_x) & 1) | (((pcolor[1] >> off_x) << 1) & 2);

    // if no OBJ (or OBJ disabled) always draw BG, otherwise check priority
    if (sprite < 0 || !ctx->video.show_obj || (bg_max_pri && ci))
        ctx->fb[fb_pos] = palette[ci];
    else 
        ctx->fb[fb_pos] = ctx->video.line_col[x];
}

// ----------------------------------------------------------------------------
INLINE void set_stat_mode(gbx_context_t *ctx, int mode)
{
    assert(mode >= 0 && mode <= 3);
    ctx->video.stat = (ctx->video.stat & ~STAT_MODE) | mode;
    log_spew("LCD STAT mode changing to %d\n", mode);
}

// ----------------------------------------------------------------------------
INLINE void check_coincidence(gbx_context_t *ctx)
{
    // fire an interrupt when LY == LYC becomes true and interrupt enabled
    if (ctx->video.lcd_y == ctx->video.lyc && (ctx->video.stat & STAT_INT_LYC))
        gbx_req_interrupt(ctx, INT_LCDSTAT);

    // this is only called when Y transitions, so rather than set the STAT LYC
    // flag here, it is computed on demand when the STAT register is read
}

// ----------------------------------------------------------------------------
INLINE void transition_to_search(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_SEARCH;
    ctx->video.cycle = 0;
    ctx->video.lcd_x = 0;

    // check for OAM STAT interrupt
    set_stat_mode(ctx, MODE_SEARCH);
    if (ctx->video.stat & STAT_INT_OAM)
        gbx_req_interrupt(ctx, INT_LCDSTAT);

    // prepare the sprites for this scanline
    prepare_line_buffer(ctx);
}

// ----------------------------------------------------------------------------
INLINE void transition_to_transfer(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_TRANSFER;
    ctx->video.cycle = 0;

    // no SEARCH STAT interrupt
    set_stat_mode(ctx, MODE_TRANSFER);
}

// ----------------------------------------------------------------------------
INLINE void transition_to_hblank(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_HBLANK;
    ctx->video.cycle = 0;

    // if there is an HBLANK DMA pending, perform a 16 byte transfer
    if (ctx->video.hdma_active)
        hdma_hblank_transfer_block(ctx);

    // check for HBLANK STAT interrupt
    set_stat_mode(ctx, MODE_HBLANK);
    if (ctx->video.stat & STAT_INT_HBLANK)
        gbx_req_interrupt(ctx, INT_LCDSTAT);
}

// ----------------------------------------------------------------------------
INLINE void transition_to_vblank(gbx_context_t *ctx)
{
    ctx->video.state = VIDEO_STATE_VBLANK;
    ctx->video.cycle = 0;

    ext_video_sync(ctx->userdata);
    gbx_req_interrupt(ctx, INT_VBLANK);

    // check for VBLANK STAT interrupt
    set_stat_mode(ctx, MODE_VBLANK);
    if (ctx->video.stat & STAT_INT_VBLANK)
        gbx_req_interrupt(ctx, INT_LCDSTAT);

    ctx->frame_cycles = 0;
}

// ----------------------------------------------------------------------------
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
                video_render_pixel(ctx, ctx->video.lcd_x, ctx->video.lcd_y);
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
                    ctx->video.curr_wy = 0;
                }
                else
                    ctx->video.cycle = 0;

                // check for coincidence interrupt each time LY changes
                check_coincidence(ctx);
            }
            break;
        }
    }
}

// ----------------------------------------------------------------------------
void gbx_get_tile_buffer(gbx_context_t *ctx, uint32_t *dest, int index)
{
    int x, y, off_x, off_y, tile, ci, pal_num = 0;
    uint8_t *cb = NULL, *char_base = NULL;

    uint32_t *palette = NULL;
    if (ctx->color_enabled)
        palette = &ctx->video.bcpd_rgb[pal_num << 2];
    else
        palette = ctx->video.bgp_rgb;

    switch (index) {
    default:
    case 0: // bank 0 region 0
        char_base = &ctx->mem.vram[0x0000];
        break;
    case 1: // bank 0 region 1
        char_base = &ctx->mem.vram[0x0800];
        break;
    case 2: // bank 1 region 0
        char_base = &ctx->mem.vram[0x0000 + VRAM_BANK_SIZE];
        break;
    case 3: // bank 1 region 1
        char_base = &ctx->mem.vram[0x0800 + VRAM_BANK_SIZE];
        break;
    }

    for (y = 0; y < GBX_LCD_YRES; y++) {
        for (x = 0; x < GBX_LCD_XRES; x++) {

            if (y >= 128 || x >= 128) {
                dest[y * GBX_LCD_XRES + x] = 0x00000000;
                continue;
            }

            tile = ((y >> 3) << 5) + (x >> 3);
            off_x = 7 - (x & 7);
            off_y = y & 7;

            cb = &char_base[(tile << 4) + (off_y << 1)];
            ci = ((cb[0] >> off_x) & 1) | (((cb[1] >> off_x) << 1) & 2);
            dest[y * GBX_LCD_XRES + x] = palette[ci];
        }
    }
}

// ----------------------------------------------------------------------------
void gbx_get_tmap_buffer(gbx_context_t *ctx, uint32_t *dest, int index)
{
    int ci, x, y;
    uint8_t *pcode, *pchar, *code_base = NULL;
    uint32_t *palette = ctx->video.bgp_rgb;

    switch (index) {
    default:
    case 0: case 1: // map 0 region 0/1
        code_base = &ctx->mem.vram[0x1800];
        break;
    case 2: case 3: // map 1 region 0/1
        code_base = &ctx->mem.vram[0x1C00];
        break;
    }

    for (y = 0; y < GBX_LCD_YRES; y++) {
        for (x = 0; x < GBX_LCD_XRES; x++) {
            int off_x = 7 - (x & 7);
            int off_y = y & 7;
            int fb_pos = y * GBX_LCD_XRES + x;
            pchar = ctx->mem.vram;
            pcode = &code_base[((y & 0xF8) << 2) | (x >> 3)];

            if (ctx->color_enabled) {
                uint8_t attr = *(pcode + VRAM_BANK_SIZE);
                if (attr & BG_ATTR_BANK) pchar += VRAM_BANK_SIZE;
                if (attr & BG_ATTR_XFLIP) off_x = 7 - off_x;
                if (attr & BG_ATTR_YFLIP) off_y = 7 - off_y;
                palette = &ctx->video.bcpd_rgb[(attr & BG_ATTR_PAL) << 2];
            }

            if (index & 1)
                pchar += (*pcode << 4) + (off_y << 1);
            else
                pchar += 0x1000 + ((int8_t)*pcode << 4) + (off_y << 1);

            ci = ((pchar[0] >> off_x) & 1) | (((pchar[1] >> off_x) << 1) & 2);
            dest[fb_pos] = palette[ci];
        }
    }
}

