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

// OAM attribute fields

#define OAM_ATTR_CPAL   0x07    // CGB only, palette number OBP0-7
#define OAM_ATTR_BANK   0x08    // CGB only, tile VRAM bank
#define OAM_ATTR_PAL    0x10    // monochrome palette number (non-CGB only)
#define OAM_ATTR_XFLIP  0x20    // mirror horizontally
#define OAM_ATTR_YFLIP  0x40    // mirror vertically
#define OAM_ATTR_PRI    0x80    // OBJ/BG priority

// background/window map attribute fields

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

#endif // GBOY_VIDEO__H

