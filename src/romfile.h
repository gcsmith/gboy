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

#ifndef GBOY_ROMFILE__H
#define GBOY_ROMFILE__H

#include "common.h"

// ROM header fields

#define GBHDR_ENTRY         0x100   // program entry point
#define GBHDR_LOGO          0x104   // nintendo logo
#define GBHDR_TITLE         0x134   // uppercase game title
#define GBHDR_MFR_CODE      0x13F   // manufacturer code (or title)
#define GBHDR_CGB_FLAG      0x143   // gameboy color flags (or title)
#define GBHDR_LIC_CODE_EX   0x144   // new licensee code
#define GBHDR_SGB_FLAG      0x146   // super gameboy flags
#define GBHDR_CART_TYPE     0x147   // cartridge type
#define GBHDR_ROM_SIZE      0x148   // length of cartridge ROM
#define GBHDR_RAM_SIZE      0x149   // length of cartridge RAM
#define GBHDR_DST_CODE      0x14A   // destination code
#define GBHDR_LIC_CODE      0x14B   // old licensee code
#define GBHDR_VERSION_NUM   0x14C   // mask rom version number
#define GBHDR_HDR_CHKSUM    0x14D   // one byte header checksum
#define GBHDR_GLB_CHKSUM    0x14E   // two byte global checksum
#define GBHDR_LENGTH        0x150   // total length of the cartridge header

// CGB field flags

#define CGB_ENABLE          0x80    // game supports CGB functions
#define CGB_COLOR_ONLY      0x40    // game works only on CGB

// SGB field flags

#define SGB_ENABLE          0x03    // game supports SGB functions

// cart feature flags (for convenience, do NOT correspond to header field)

#define CART_RAM            0x01    // external RAM present
#define CART_BATTERY        0x02    // RAM is battery backed
#define CART_TIMER          0x04    // external timer present
#define CART_RUMBLE         0x08    // rumble support present
#define CART_MBC            0xF0    // mask for MBC type identifier
#define CART_MBC_ROM        0x00    // no memory bank controller (ROM only)
#define CART_MBC_MBC1       0x10
#define CART_MBC_MBC2       0x20
#define CART_MBC_MBC3       0x30
#define CART_MBC_MBC4       0x40
#define CART_MBC_MBC5       0x50
#define CART_MBC_MBC6       0x60
#define CART_MBC_MBC7       0x70
#define CART_MBC_MMM01      0x80
#define CART_MBC_HuC1       0x90
#define CART_MBC_HuC3       0xA0
#define CART_MBC_PCAM       0xB0    // pocket camera
#define CART_MBC_TAMA5      0xC0

typedef struct rom_header {
    char     title[17];
    char     license[2];
    uint8_t  cgb_flag;
    uint8_t  sgb_flag;
    uint8_t  carttype;
    uint8_t  rom_size;
    uint8_t  ram_size;
    uint8_t  dst_code;
    uint8_t  lic_code;
    uint8_t  version;
    uint8_t  chksum_h, computed_h;
    uint16_t chksum_g, computed_g;
} rom_header_t;

void rom_extract_header(rom_header_t *rh, uint8_t *buffer, size_t length);
void rom_print_details(rom_header_t *rh);

int rom_get_cart_features(int carttype);
int xrom_size_to_banks(int rom_size);
int xram_size_to_banks(int ram_size);

#endif // GBOY_ROMFILE__H

