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
#include "romfile.h"

typedef struct code_table {
    uint8_t code;
    const char *str;
} code_table_t;

static code_table_t old_licensee[] = {
    { 0x00, "None" },
    { 0x01, "Nintendo" },
    { 0x08, "Capcom" },
    { 0x09, "Hot-B" },
    { 0x0A, "Jaleco" },
    { 0x0B, "Coconuts" },
    { 0x0C, "Elite Systems" },
    { 0x13, "Electronic Arts" },
    { 0x18, "Hudson Soft" },
    { 0x19, "ITC Entertainment" },
    { 0x1A, "Yanoman" },
    { 0x1D, "Clary" },
    { 0x1F, "Virgin" },
    { 0x24, "PCM Complete" },
    { 0x25, "San-X" },
    { 0x28, "Kotobuki Systems" },
    { 0x29, "Seta" },
    { 0x30, "Infogrames" },
    { 0x31, "Nintendo" },
    { 0x32, "Bandai" },
    { 0x33, "Bandai" },
    { 0x34, "Konami" },
    { 0x35, "Hector" },
    { 0x38, "Capcom" },
    { 0x39, "Banpresto" },
    { 0x3C, "*entertainment i" },
    { 0x3E, "Gremlin" },
    { 0x41, "Ubi soft" },
    { 0x42, "Atlus" },
    { 0x44, "Malibu" },
    { 0x46, "Angel" },
    { 0x47, "Spectrum Holobyte" },
    { 0x49, "Irem" },
    { 0x4A, "Virgin" },
    { 0x4D, "Malibu" },
    { 0x4F, "U.s. gold" },
    { 0x50, "Absolute" },
    { 0x51, "Acclaim" },
    { 0x52, "Activision" },
    { 0x53, "American sammy" },
    { 0x54, "Gametek" },
    { 0x55, "Park place" },
    { 0x56, "Ljn" },
    { 0x57, "Matchbox" },
    { 0x59, "Milton bradley" },
    { 0x5A, "Mindscape" },
    { 0x5B, "Romstar" },
    { 0x5C, "Naxat soft" },
    { 0x5D, "Tradewest" },
    { 0x60, "Titus" },
    { 0x61, "Virgin" },
    { 0x67, "Ocean" },
    { 0x69, "Electronic arts" },
    { 0x6E, "Elite systems" },
    { 0x6F, "Electro brain" },
    { 0x70, "Infogrames" },
    { 0x71, "Interplay" },
    { 0x72, "Broderbund" },
    { 0x73, "Sculptered soft" },
    { 0x75, "The sales curve" },
    { 0x78, "T*hq" },
    { 0x79, "Accolade" },
    { 0x7A, "Triffix entertainment" },
    { 0x7C, "Microprose" },
    { 0x7F, "Kemco" },
    { 0x80, "Misawa entertainment" },
    { 0x83, "Lozc" },
    { 0x86, "*Tokuma shoten i" },
    { 0x8B, "Bullet-Proof Software" },
    { 0x8C, "Vic tokai" },
    { 0x8E, "Ape" },
    { 0x8F, "I'max" },
    { 0x91, "Chun soft" },
    { 0x92, "Video system" },
    { 0x93, "Tsuburava" },
    { 0x95, "Varie" },
    { 0x96, "Yonezawa/s'pal" },
    { 0x97, "Kaneko" },
    { 0x99, "Arc" },
    { 0x9A, "Nihon bussan" },
    { 0x9B, "Tecmo" },
    { 0x9C, "Imagineer" },
    { 0x9D, "Banpresto" },
    { 0x9F, "Nova" },
    { 0xA1, "Hori electric" },
    { 0xA2, "Bandai" },
    { 0xA4, "Konami" },
    { 0xA6, "Kawada" },
    { 0xA7, "Takara" },
    { 0xA9, "Technos japan" },
    { 0xAA, "Broderbund" },
    { 0xAC, "Toei animation" },
    { 0xAD, "Toho" },
    { 0xAF, "Namco" },
    { 0xB0, "Acclaim" },
    { 0xB1, "Ascii or nexoft" },
    { 0xB2, "Bandai" },
    { 0xB4, "Enix" },
    { 0xB6, "Hal" },
    { 0xB7, "Snk" },
    { 0xB9, "Pony Canyon" },
    { 0xBA, "*Culture brain o" },
    { 0xBB, "Sunsoft" },
    { 0xBD, "Sony imagesoft" },
    { 0xBF, "Sammy" },
    { 0xC0, "Taito" },
    { 0xC2, "Kemco" },
    { 0xC3, "Squaresoft" },
    { 0xC4, "*Tokuma shoten i" },
    { 0xC5, "Data east" },
    { 0xC6, "Tonkin house" },
    { 0xC8, "Koei" },
    { 0xC9, "Ufl" },
    { 0xCA, "Ultra" },
    { 0xCB, "Vap" },
    { 0xCC, "Use" },
    { 0xCD, "Meldac" },
    { 0xCE, "Pony Canyon" },
    { 0xCF, "Angel" },
    { 0xD0, "Taito" },
    { 0xD1, "Sofel" },
    { 0xD2, "Quest" },
    { 0xD3, "Sigma enterprises" },
    { 0xD4, "Ask kodansha" },
    { 0xD6, "Naxat soft" },
    { 0xD7, "Copya systems" },
    { 0xD9, "Banpresto" },
    { 0xDA, "Tomy" },
    { 0xDB, "Ljn" },
    { 0xDD, "Ncs" },
    { 0xDE, "Human" },
    { 0xDF, "Altron" },
    { 0xE0, "Jaleco" },
    { 0xE1, "Towachiki" },
    { 0xE2, "Uutaka" },
    { 0xE3, "Varie" },
    { 0xE5, "Epoch" },
    { 0xE7, "Athena" },
    { 0xE8, "Asmik" },
    { 0xE9, "Natsume" },
    { 0xEA, "King records" },
    { 0xEB, "Atlus" },
    { 0xEC, "Epic/sony records" },
    { 0xEE, "Igs" },
    { 0xF0, "A-Wave" },
    { 0xF3, "Extreme Entertainment" },
    { 0xFF, "Ljn" },
    { 0x00, NULL }
};

static code_table_t new_licensee[] = {
    { 0x00, "None" },
    { 0x01, "Nintendo" },
    { 0x08, "Capcom" },
    { 0x13, "Electronic arts" },
    { 0x18, "Hudsonsoft" },
    { 0x19, "B-ai" },
    { 0x20, "Kss" },
    { 0x22, "Pow" },
    { 0x24, "Pcm complete" },
    { 0x25, "San-x" },
    { 0x28, "Kemco japan" },
    { 0x29, "Seta" },
    { 0x30, "Viacom" },
    { 0x31, "Nintendo" },
    { 0x32, "Bandia" },
    { 0x33, "Ocean/acclaim" },
    { 0x34, "Konami" },
    { 0x35, "Hector" },
    { 0x37, "Taito" },
    { 0x38, "Hudson" },
    { 0x39, "banpresto" },
    { 0x41, "Ubi soft" },
    { 0x42, "Atlus" },
    { 0x44, "Malibu" },
    { 0x46, "Angel" },
    { 0x47, "Bullet-Proof" },
    { 0x49, "Irem" },
    { 0x50, "Absolute" },
    { 0x51, "Acclaim" },
    { 0x52, "Activision" },
    { 0x53, "American sammy" },
    { 0x54, "Konami" },
    { 0x55, "Hi tech entertainment" },
    { 0x56, "Ljn" },
    { 0x57, "Matchbox" },
    { 0x58, "Mattel" },
    { 0x59, "Milton bradley" },
    { 0x60, "Titus" },
    { 0x61, "Virgin" },
    { 0x64, "Lucasarts" },
    { 0x67, "Ocean" },
    { 0x69, "Electronic arts" },
    { 0x70, "Infogrames" },
    { 0x71, "Interplay" },
    { 0x72, "Broderbund" },
    { 0x73, "Sculptured" },
    { 0x75, "Sci" },
    { 0x78, "T*hq" },
    { 0x79, "Accolade" },
    { 0x80, "Misawa" },
    { 0x83, "Lozc" },
    { 0x86, "Tokuma shoten i*" },
    { 0x87, "Tsukuda ori*" },
    { 0x91, "Chun soft" },
    { 0x92, "Video system" },
    { 0x93, "Ocean/acclaim" },
    { 0x95, "Varie" },
    { 0x96, "Yonezawa/s'pal" },
    { 0x97, "Kaneko" },
    { 0x99, "Pack in soft" },
    { 0x00, NULL },
};

static code_table_t cgb_flag[] = {
    { 0x00, "None" },
    { 0x80, "CGB + GB" },
    { 0xC0, "CGB Only" },
    { 0x00, NULL },
};

static code_table_t sgb_flag[] = {
    { 0x00, "None" },
    { 0x03, "SGB Supported" },
    { 0x00, NULL }
};

static code_table_t cart_type[] = {
    { 0x00, "ROM only" },
    { 0x01, "MBC1" },
    { 0x02, "MBC1 + RAM" },
    { 0x03, "MBC1 + RAM + BATTERY" },
    { 0x05, "MBC2" },
    { 0x06, "MBC2 + BATTERY" },
    { 0x08, "ROM + RAM" },
    { 0x09, "ROM + RAM + BATTERY" },
    { 0x0B, "MMM01" },
    { 0x0C, "MMM01 + RAM" },
    { 0x0D, "MMM01 + RAM + BATTERY" },
    { 0x0F, "MBC3 + TIMER + BATTERY" },
    { 0x10, "MBC3 + TIMER + RAM + BATTERY" },
    { 0x11, "MBC3" },
    { 0x12, "MBC3 + RAM" },
    { 0x13, "MBC3 + RAM + BATTERY" },
    { 0x15, "MBC4" },
    { 0x16, "MBC4 + RAM" },
    { 0x17, "MBC4 + RAM + BATTERY" },
    { 0x19, "MBC5" },
    { 0x1A, "MBC5 + RAM" },
    { 0x1B, "MBC5 + RAM + BATTERY" },
    { 0x1C, "MBC5 + RUMBLE" },
    { 0x1D, "MBC5 + RUMBLE + RAM" },
    { 0x1E, "MBC5 + RUMBLE + RAM + BATTERY" },
    { 0xFC, "POCKET CAMERA" },
    { 0xFD, "BANDAI TAMA5" },
    { 0xFE, "HuC3" },
    { 0xFF, "HuC1 + RAM + BATTERY" },
    { 0x00, NULL }
};

static code_table_t destination[] = {
    { 0x00, "Japanese" },
    { 0x01, "Non-Japanese" },
    { 0x00, NULL }
};

static code_table_t rom_type[] = {
    { 0x00, "32 KB, no banking" },
    { 0x01, "64 KB, 4 banks" },
    { 0x02, "128 KB, 8 banks" },
    { 0x03, "256 KB, 16 banks" },
    { 0x04, "512 KB, 32 banks" },
    { 0x05, "1 MB, 64 banks" },
    { 0x06, "2 MB, 128 banks" },
    { 0x07, "4 MB, 256 banks" },
    { 0x52, "1.1 MB, 72 banks" },
    { 0x53, "1.2 MB, 80 banks" },
    { 0x54, "1.5 MB, 96 banks" },
    { 0x00, NULL }
};

static code_table_t ram_type[] = {
    { 0x00, "None" },
    { 0x01, "2 KB, no banking" },
    { 0x02, "8 KB, no banking" },
    { 0x03, "32 KB, 4 banks" },
    { 0x04, "128 KB, 16 banks" },
    { 0x00, NULL }
};

// -----------------------------------------------------------------------------
static const char *search_table(const code_table_t *tab, uint8_t code)
{
    int index = 0;
    for (;;) {
        if (!tab[index].code && !tab[index].str)
            break;
        if (tab[index].code == code)
            return tab[index].str;
        ++index;
    }
    return "unknown";
}

// -----------------------------------------------------------------------------
void rom_extract_header(rom_header_t *rh, uint8_t *buffer, size_t length)
{
    size_t i;

    rh->cgb_flag = buffer[GBHDR_CGB_FLAG];
    rh->sgb_flag = buffer[GBHDR_SGB_FLAG];
    rh->carttype = buffer[GBHDR_CART_TYPE];
    rh->rom_size = buffer[GBHDR_ROM_SIZE];
    rh->ram_size = buffer[GBHDR_RAM_SIZE];
    rh->dst_code = buffer[GBHDR_DST_CODE];
    rh->lic_code = buffer[GBHDR_LIC_CODE];
    rh->version  = buffer[GBHDR_VERSION_NUM];
    rh->chksum_h = buffer[GBHDR_HDR_CHKSUM];
    rh->chksum_g = buffer[GBHDR_GLB_CHKSUM] << 8 | buffer[GBHDR_GLB_CHKSUM + 1];

    rh->license[0] = buffer[GBHDR_LIC_CODE_EX];
    rh->license[1] = buffer[GBHDR_LIC_CODE_EX + 1];

    // compute the header and global checksum values
    rh->computed_h = 0;
    for (i = 0x134; i <= 0x14C; ++i)
        rh->computed_h -= (buffer[i] + 1);

    rh->computed_g = 0;
    for (i = 0; i < length; i++)
        if (i < 0x14E || i > 0x14F) rh->computed_g += buffer[i];

    memcpy(rh->title, &buffer[GBHDR_TITLE], 16);

    // determine if this is a newer cart, if so display the shortened title
    if (rh->cgb_flag & CGB_ENABLE) {
        rh->title[15] = '\0';
    }
    else {
        rh->title[16] = '\0';
        rh->cgb_flag = 0;
    }
}

// -----------------------------------------------------------------------------
void rom_print_details(rom_header_t *rh)
{
    log_info("  Title:        %s\n", rh->title);
    log_info("  Version:      %02X\n", rh->version);

    // determine the type of and display licensee code
    if (rh->lic_code == 0x33) {
        uint8_t code = (rh->license[0] - '0') << 8 | (rh->license[1] - '0');
        log_info("  LicenseeEx:   %02X    %s\n", code,
                 search_table(new_licensee, code));
    }
    else
        log_info("  Licensee:     %02X    %s\n", rh->lic_code,
                 search_table(old_licensee, rh->lic_code));

    // dump the rest of the rom header fields
    log_info("  CGB Flags:    %02X    %s\n", rh->cgb_flag,
             search_table(cgb_flag, rh->cgb_flag));
    log_info("  SGB Flags:    %02X    %s\n", rh->sgb_flag,
             search_table(sgb_flag, rh->sgb_flag));
    log_info("  Cart Type:    %02X    %s\n", rh->carttype,
             search_table(cart_type, rh->carttype));
    log_info("  Destination:  %02X    %s\n", rh->dst_code,
             search_table(destination, rh->dst_code));
    log_info("  ROM Size:     %02X    %s\n", rh->rom_size,
             search_table(rom_type, rh->rom_size));
    log_info("  RAM Size:     %02X    %s\n", rh->ram_size,
             search_table(ram_type, rh->ram_size));
    log_info("  H. Checksum:  %02X    %s\n", rh->chksum_h,
             (rh->computed_h == rh->chksum_h) ? "OK" : "BAD");
    log_info("  G. Checksum:  %04X  %s\n", rh->chksum_g,
             (rh->computed_g == rh->chksum_g) ? "OK" : "BAD");
}

// -----------------------------------------------------------------------------
int rom_get_cart_features(int carttype)
{
    switch (carttype) {
    case 0x00: return CART_MBC_ROM;
    case 0x01: return CART_MBC_MBC1;
    case 0x02: return CART_MBC_MBC1 | CART_RAM;
    case 0x03: return CART_MBC_MBC1 | CART_RAM | CART_BATTERY ;
    case 0x05: return CART_MBC_MBC2;
    case 0x06: return CART_MBC_MBC2 | CART_BATTERY;
    case 0x08: return CART_MBC_ROM | CART_RAM;
    case 0x09: return CART_MBC_ROM | CART_RAM | CART_BATTERY;
    case 0x0B: return CART_MBC_MMM01;
    case 0x0C: return CART_MBC_MMM01 | CART_RAM;
    case 0x0D: return CART_MBC_MMM01 | CART_RAM | CART_BATTERY;
    case 0x0F: return CART_MBC_MBC3 | CART_TIMER | CART_BATTERY;
    case 0x10: return CART_MBC_MBC3 | CART_TIMER | CART_RAM | CART_BATTERY;
    case 0x11: return CART_MBC_MBC3;
    case 0x12: return CART_MBC_MBC3 | CART_RAM;
    case 0x13: return CART_MBC_MBC3 | CART_RAM | CART_BATTERY;
    case 0x15: return CART_MBC_MBC4;
    case 0x16: return CART_MBC_MBC4 | CART_RAM;
    case 0x17: return CART_MBC_MBC4 | CART_RAM | CART_BATTERY;
    case 0x19: return CART_MBC_MBC5;
    case 0x1A: return CART_MBC_MBC5 | CART_RAM;
    case 0x1B: return CART_MBC_MBC5 | CART_RAM | CART_BATTERY;
    case 0x1C: return CART_MBC_MBC5 | CART_RUMBLE;
    case 0x1D: return CART_MBC_MBC5 | CART_RUMBLE | CART_RAM;
    case 0x1E: return CART_MBC_MBC5 | CART_RUMBLE | CART_RAM | CART_BATTERY;
    case 0xFC: return CART_MBC_CAMERA;
    case 0xFD: return CART_MBC_TAMA5;
    case 0xFE: return CART_MBC_HuC3;
    case 0xFF: return CART_MBC_HuC1 | CART_RAM | CART_BATTERY;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Determine the number of ROM banks based on the ROM size header field.
int xrom_size_to_banks(int rom_size)
{
    switch (rom_size) {
    case 0x00: case 0x01: case 0x02: case 0x03:
    case 0x04: case 0x05: case 0x06: case 0x07:
        return 2 << rom_size;
    case 0x52: return 72;
    case 0x53: return 80;
    case 0x54: return 96;
    }
    return -1;
}

// -----------------------------------------------------------------------------
// Determine the number of RAM banks based on the RAM size header field.
int xram_size_to_banks(int ram_size)
{
    switch (ram_size) {
    case 0x00: return 0;
    case 0x01: return 1;
    case 0x02: return 1;
    case 0x03: return 4;
    }
    return -1;
}

