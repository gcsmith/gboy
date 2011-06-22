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

#ifndef GBOY_VIDEO__H
#define GBOY_VIDEO__H

// Gameboy Speed:   4.194304 MHz
// Horizontal Sync: 9198 Hz (108.719287 us)
// Vertical Sync:   59.73 Hz (16.7420057 ms)

// Refresh Cycles:  70224 (17556)
// VBLANK Cycles:   4560 (1140)

// Total Lines:     154 (LY =   0..153)
// Visible Lines:   144 (LY =   0..143)
// VBLANK Lines:    10  (LY = 144..153)

// Cycles Per Line: 4560 / 10 = 456 (114)
// Visible Pixels:  160 (40)
// Mode 0 Cycles:   204 (51)    reported 201-207
// Mode 2 Cycles:   80  (20)    reported 77-83
// Mode 3 Cycles:   172 (43)    reported 169-175

#include "common.h"

// LCD screen resolution, identical for all game boy variants (before GBA)

#define GBX_LCD_XRES    160
#define GBX_LCD_YRES    144

// LCD control (LCDC) fields

#define LCDC_BG_EN      0x01    // background display enable
#define LCDC_OBJ_EN     0x02    // object display enable
#define LCDC_OBJ_SIZE   0x04    // object size
#define LCDC_BG_CODE    0x08    // background tile map select
#define LCDC_BG_CHAR    0x10    // background & window title data select
#define LCDC_WND_EN     0x20    // window display enable
#define LCDC_WND_CODE   0x40    // window tile map select
#define LCDC_LCD_EN     0x80    // LCD display enable

// LCDC status (STAT) fields

#define STAT_MODE       0x03    // mode flag (mode 0-3)
#define STAT_LYC        0x04    // coincidence flag (LYC ?= LY)
#define STAT_INT_HBLANK 0x08    // mode 0 h-blank interrupt enable
#define STAT_INT_VBLANK 0x10    // mode 1 v-blank interrupt enable
#define STAT_INT_OAM    0x20    // mode 2 oam interrupt enable
#define STAT_INT_LYC    0x40    // coincidence interrupt enable

#define MODE_HBLANK     0x00
#define MODE_VBLANK     0x01
#define MODE_SEARCH     0x02
#define MODE_TRANSFER   0x03

// color palette specification fields

#define CPS_INDEX       0x3F
#define CPS_INCREMENT   0x80

// HDMA control fields

#define HDMA_LENGTH     0x7F    // DMA transfer length
#define HDMA_TYPE       0x80    // DMA type (general purpose or h-blank)

// OAM attribute fields

#define OAM_ATTR_CPAL   0x07    // CGB only, palette number OBP0-7
#define OAM_ATTR_BANK   0x08    // CGB only, tile VRAM bank
#define OAM_ATTR_PAL    0x10    // monochrome palette number (non-CGB only)
#define OAM_ATTR_XFLIP  0x20    // mirror horizontally
#define OAM_ATTR_YFLIP  0x40    // mirror vertically
#define OAM_ATTR_PRI    0x80    // OBJ/BG priority

// background/window map attribute fields (CGB mode only)

#define BG_ATTR_PAL     0x07
#define BG_ATTR_BANK    0x08
#define BG_ATTR_XFLIP   0x20
#define BG_ATTR_YFLIP   0x40
#define BG_ATTR_PRI     0x80

// video controller state and cycle counts

#define VIDEO_STATE_SEARCH      0
#define VIDEO_STATE_TRANSFER    1
#define VIDEO_STATE_HBLANK      2
#define VIDEO_STATE_VBLANK      3

#define VIDEO_CYCLES_SEARCH     80
#define VIDEO_CYCLES_TRANSFER   172
#define VIDEO_CYCLES_HBLANK     204
#define VIDEO_CYCLES_VBLANK     4560
#define VIDEO_CYCLES_SCANLINE   456
#define VIDEO_CYCLES_TOTAL      70224

#define LCD_SCANLINE_COUNT      154

typedef struct obj_char {
    uint8_t ypos;               // y-axis coordinate
    uint8_t xpos;               // x-axis coordinate
    uint8_t code;               // character code
    uint8_t attr;               // attribute data
} obj_char_t;

typedef struct video_registers {
    int line_obj[GBX_LCD_XRES]; // object lookup for each column in scanline
    int line_col[GBX_LCD_XRES];
    int show_bg;                // enable display of background
    int show_wnd;               // enable display of window
    int show_obj;               // enable display of objects
    int obj_pri;                // enable forced object priority over bg/wnd
    int state, cycle;
    int lcdc, stat, lyc;
    int lcd_x, lcd_y;
    int scy, scx;
    int wx, wy;
    int bgp, obp0, obp1;
    uint32_t bgp_rgb[4];
    uint32_t obp0_rgb[4];
    uint32_t obp1_rgb[4];
    int bcps, ocps;
    uint8_t bcpd[0x40];
    uint8_t ocpd[0x40];
    uint32_t bcpd_rgb[0x20];
    uint32_t ocpd_rgb[0x20];
    uint16_t hdma_src, hdma_dst;
    int hdma_len, hdma_active, hdma_pos;
} video_registers_t;

// ----------------------------------------------------------------------------
INLINE uint16_t video_validate_hdma_src(uint16_t addr)
{
    addr &= 0xFFF0;
    if (addr <= 0x7FF0 || (addr >= 0xA000 && addr <= 0xDFF0))
        return addr;

    // not sure how to handle this case, so set to 0000 and report error
    log_err("invalid HDMA source address %04X specified. set to 0000\n");
    return 0;
}

// ----------------------------------------------------------------------------
INLINE uint16_t video_validate_hdma_dst(uint16_t addr)
{
    addr = (addr & 0x1FF0) | 0x8000;
    if (addr >= 0x8000 && addr <= 0x9FF0)
        return addr;

    // not sure how to handle this case, so set to 0000 and report error
    log_err("invalid HDMA destination address %04X specified. set to 0000\n");
    return 0;
}

// ----------------------------------------------------------------------------
INLINE uint32_t cgb_color_to_rgb(uint16_t c)
{
    return ((c & 0x001F) << 3) | ((c & 0x03E0) << 6) | ((c & 0x7C00) << 9);
}

void video_write_mono_palette(uint32_t *dest, uint8_t value);
void video_write_lcdc(gbx_context_t *ctx, uint8_t value);
void video_write_bcpd(gbx_context_t *ctx, uint8_t value);
void video_write_ocpd(gbx_context_t *ctx, uint8_t value);
void video_write_hdma(gbx_context_t *ctx, uint8_t value);
uint8_t video_read_hdma(gbx_context_t *ctx);
void video_update_cycles(gbx_context_t *ctx, long cycles);

#endif // GBOY_VIDEO__H

