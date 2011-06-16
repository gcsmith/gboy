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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gbx.h"
#include "memory.h"
#include "ports.h"

// -----------------------------------------------------------------------------
int gbx_create_context(gbx_context_t **pctx, int system)
{
    gbx_context_t *ctx;

    // validate the requsted system type
    if (system < SYSTEM_GMB || system > SYSTEM_AUTO) {
        log_err("Invalid system mode (%d) specified.\n", system);
        return -1;
    }

    // create and initialize the context structure
    ctx = (gbx_context_t *)calloc(1, sizeof(gbx_context_t));
    ctx->system = system;
    ctx->input_state = 0xFF;

    // initialize LCD controller
    ctx->video.lcd_x = 0;
    ctx->video.lcd_y = GBX_LCD_YRES;
    ctx->video.state = VIDEO_STATE_VBLANK;

    *pctx = ctx;
    return 0;
}

// -----------------------------------------------------------------------------
void gbx_destroy_context(gbx_context_t *ctx)
{
    if (!ctx) return;

    SAFE_FREE(ctx->mem.bios);
    SAFE_FREE(ctx->mem.wram);
    SAFE_FREE(ctx->mem.vram);
    SAFE_FREE(ctx->mem.xram);
    SAFE_FREE(ctx->mem.xrom);
    SAFE_FREE(ctx);
}

// -----------------------------------------------------------------------------
static int load_binary_file(const char *path, uint8_t **pbuf, size_t *plen)
{
    FILE *fp;
    size_t bytes_read;

    // open the rom image for reading in binary mode
    if (NULL == (fp = fopen(path, "rb"))) {
        log_err("Unable to open file for reading.\n");
        return -1;
    }

    // determine the file length and allocate an appropriately sized buffer
    fseek(fp, 0, SEEK_END);
    *plen = ftell(fp);
    *pbuf = malloc(*plen);
    fseek(fp, 0, SEEK_SET);

    // read the file contents and verify the correct number of bytes were read
    bytes_read = fread(*pbuf, 1, *plen, fp);
    fclose(fp);

    if (*plen != bytes_read) {
        log_err("File io error (read %d of %d bytes).\n", bytes_read, *plen);
        SAFE_FREE(*pbuf);
        return -1;
    }

    log_info("Successfully loaded file \"%s\".\n", path);
    return 0;
}

// -----------------------------------------------------------------------------
static int detect_system_type(rom_header_t *header)
{
    // if game supports color mode, emulate gameboy color
    if (header->cgb_flag & CGB_ENABLE) {
        log_info("Automatically enabling CGB system support.\n");
        return SYSTEM_CGB;
    }

    // otherwise, if game supports SGB functionality, emulate super gameboy
    if (header->sgb_flag & SGB_ENABLE) {
        log_info("Automatically enabling SGB system support.\n");
        return SYSTEM_SGB;
    }

    // if game supports neither color nor SGB, emulate the standard gameboy
    log_info("Automatically enabling GMB system support.\n");
    return SYSTEM_GMB;
}

// -----------------------------------------------------------------------------
static int process_header_fields(gbx_context_t *ctx, rom_header_t *header)
{
    // make sure that the ROM and RAM size fields make sense
    if (0 > (ctx->mem.xrom_banks = xrom_size_to_banks(header->rom_size))) {
        log_err("unexpected rom size specified\n");
        return -1;
    }

    if (0 > (ctx->mem.xram_banks = xram_size_to_banks(header->ram_size))) {
        log_err("uxnexpected ram size specified\n");
        return -1;
    }

    // if game requires color, we can't run in any other mode beside CGB
    if ((header->cgb_flag & CGB_COLOR_ONLY) && (ctx->system != SYSTEM_CGB)) {
        log_err("Cannot run color-only game in non-CGB system mode.\n");
        return -1;
    }

    // enable CGB features only if game supports color AND system is CGB
    if ((header->cgb_flag & CGB_ENABLE) && (ctx->system == SYSTEM_CGB))
        ctx->cgb_enabled = 1;
    else
        ctx->cgb_enabled = 0;

    // game supports SGB functionality, but current mode doesn't support it
    if ((header->sgb_flag & SGB_ENABLE) &&
        (ctx->system != SYSTEM_SGB) && (ctx->system != SYSTEM_SGB2)) {
        log_info("Game supports SGB features, but they are disabled.\n");
        // this is not an error, just let the user know the option exists
    }

    // store cart feature flags (bank controller, peripherals, etc)
    ctx->cart_features = rom_get_cart_features(ctx->header.carttype);

    return 0;
}

// -----------------------------------------------------------------------------
static int alloc_memory_regions(gbx_context_t *ctx, uint8_t *rom, size_t size)
{
    size_t xrom_size, xram_size, vram_size, wram_size;

    // set the video and work ram sizes based on whether we're in GB/CGB mode
    if (ctx->system == SYSTEM_CGB) {
        log_info("Allocating address space for color gameboy...\n");
        ctx->mem.vram_banks = 2;
        ctx->mem.wram_banks = 8;
    }
    else {
        log_info("Allocating address space for monochrome gameboy...\n");
        ctx->mem.vram_banks = 1;
        ctx->mem.wram_banks = 2;
    }

    xrom_size = XROM_BANK_SIZE * ctx->mem.xrom_banks;
    xram_size = XRAM_BANK_SIZE * ctx->mem.xram_banks;
    vram_size = VRAM_BANK_SIZE * ctx->mem.vram_banks;
    wram_size = WRAM_BANK_SIZE * ctx->mem.wram_banks;

    // the file size should be equal to the combined size of the ROM banks
    if (size > xrom_size) {
        log_err("File size exceeds size reported in ROM header. Truncated.\n");
        log_err("File size: %d bytes, ROM size: %d bytes\n", size, xrom_size);
        return -1;
    }
    else if (size < xrom_size) {
        // allocate a buffer of the appropriate length and copy the image over
        uint8_t *temp_buffer = calloc(1, xrom_size);
        memcpy(temp_buffer, rom, size);
        free(rom);
        rom = temp_buffer;

        log_err("ROM size reported in header exceeds file size. Padded.\n");
        log_err("File size: %d bytes, ROM size: %d bytes\n", size, xrom_size);
    }

    // allocate each region of memory, keep track of base and banked address
    if (xrom_size) {
        ctx->mem.xrom = rom;
        ctx->mem.xrom_bank = ctx->mem.xrom + XROM_BANK_SIZE;
        ctx->mem.xrom_bnum = 1;
        log_info("  External ROM:  %d KB\n", xrom_size >> 10);
    }

    if (xram_size) {
        ctx->mem.xram = calloc(1, xram_size);
        ctx->mem.xram_bank = ctx->mem.xram;
        log_info("  External RAM:  %d KB\n", xram_size >> 10);
    }

    if (vram_size) {
        ctx->mem.vram = calloc(1, vram_size);
        ctx->mem.vram_bank = ctx->mem.vram;
        log_info("  Internal VRAM: %d KB\n", vram_size >> 10);
    }

    if (wram_size) {
        ctx->mem.wram = calloc(1, wram_size);
        ctx->mem.wram_bank = ctx->mem.wram + WRAM_BANK_SIZE;
        log_info("  Internal WRAM: %d KB\n", wram_size >> 10);
    }

    return 0;
}

// -----------------------------------------------------------------------------
static int configure_bank_controllers(gbx_context_t *ctx)
{
    switch (ctx->cart_features & CART_MBC) {
    case CART_MBC_MBC1:
        ctx->have_mbc1 = 1;
        break;
    case CART_MBC_MBC2:
        // MBC2 has a built in RAM of 512x4bits, RAM banks must be set to 00
        if (ctx->mem.xram_banks) {
            log_err("Cannot specify XRAM for MBC2, built-in memory only.\n");
            return -1;
        }
        // allocate 512 bytes, although only the low nibbles are used
        ctx->mem.xram = calloc(1, 512);
        ctx->have_mbc2 = 1;
        break;
    case CART_MBC_MBC3:
        ctx->have_mbc3 = 1;
        break;
    case CART_MBC_MBC4:
        // as far as I've been able to gather, MBC4 doesn't actually exist?
        log_err("Specified unsupported MBC4, which doesn't appear to exist.");
        return -1;
    case CART_MBC_MBC5:
        ctx->have_mbc5 = 1;
        break;
    }
    return 0;
}

// -----------------------------------------------------------------------------
void optionally_load_bios(gbx_context_t *ctx)
{
    char *path = NULL;
    const char *bios_file = NULL;
    uint8_t *buffer = NULL;
    size_t length;

    ctx->bios_enabled = 0;

    // determine which bios to load based on the system type (if supported)
    switch (ctx->system) {
    case SYSTEM_GMB: bios_file = "gmb_bios.bin"; break;
    case SYSTEM_CGB: bios_file = "cgb_bios.bin"; break;
    case SYSTEM_SGB: bios_file = "sgb_bios.bin"; break;
    default: return;
    }

    // allocate enough space to store the full path to the bios file
    length = strlen(ctx->bios_dir) + 16;
    path = malloc(length);
    snprintf(path, length, "%s/%s", ctx->bios_dir, bios_file);

    // load bios into memory, set flag to enable bios page at 0x0000-0x0100
    if (load_binary_file(path, &buffer, &length))
        log_err("bios file \"%s\" not found.\n", path);
    else {
        ctx->mem.bios = buffer;
        ctx->bios_enabled = 1;
    }

    SAFE_FREE(path);
}

// -----------------------------------------------------------------------------
int gbx_load_file(gbx_context_t *ctx, const char *path)
{
    int rc = -1;
    size_t length = 0;
    uint8_t *buffer = NULL;
    rom_header_t *header = &ctx->header;

    if (load_binary_file(path, &buffer, &length))
        goto error_cleanup;

    // if the file is smaller than the last header address, it can't be valid
    if (length < GBHDR_LENGTH) {
        log_err("file is not a valid gameboy image\n");
        goto error_cleanup;
    }

    rom_extract_header(header, buffer, length);
    rom_print_details(header);

    // determine system setting, if that hasn't been done for us
    if (ctx->system == SYSTEM_AUTO)
        ctx->system = detect_system_type(header);

    // now validate the system setting against the supported game features
    if (process_header_fields(ctx, header))
        goto error_cleanup;

    // allocate memory for XRAM/VRAM/WRAM
    if (alloc_memory_regions(ctx, buffer, length))
        goto error_cleanup;

    if (configure_bank_controllers(ctx))
        goto error_cleanup;

    // if a bios path was specified, search for bios ROM and load it if found
    if (ctx->bios_dir)
        optionally_load_bios(ctx);

    rc = 0;
    buffer = NULL;

error_cleanup:
    SAFE_FREE(buffer);
    return rc;
}

// -----------------------------------------------------------------------------
void gbx_set_userdata(gbx_context_t *ctx, void *userdata)
{
    assert(NULL != ctx);
    ctx->userdata = userdata;
}

// -----------------------------------------------------------------------------
void gbx_set_bios_dir(gbx_context_t *ctx, const char *path)
{
    assert(NULL != ctx);

    ctx->bios_dir = path;
    log_dbg("bios directory set to %s\n", path);
}

// -----------------------------------------------------------------------------
void gbx_set_debugger(gbx_context_t *ctx, int enable)
{
    assert(NULL != ctx);

    if (enable)
        ctx->exec_flags |= EXEC_TRACE;
    else
        ctx->exec_flags &= ~EXEC_TRACE;

    log_dbg("debug mode %s\n", enable ? "enabled" : "disabled");
}

// -----------------------------------------------------------------------------
void gbx_set_input_state(gbx_context_t *ctx, int key, int pressed)
{
    assert(NULL != ctx);
    assert((key >= INPUT_RIGHT) && (key <= INPUT_START));

    // joypad register bits are active low
    if (pressed)
        ctx->input_state &= ~(1 << key);
    else
        ctx->input_state |= (1 << key);

    gbx_req_interrupt(ctx, INT_JOYPAD);
    log_dbg("input index %d set to %s\n", key, pressed ? "down" : "up");
}

// -----------------------------------------------------------------------------
void gbx_get_framebuffer(gbx_context_t *ctx, void *dest)
{
    assert(NULL != ctx);
    assert(NULL != dest);
    memcpy(dest, ctx->fb, GBX_LCD_XRES * GBX_LCD_YRES * 4);
}

// -----------------------------------------------------------------------------
int gbx_get_clock_frequency(gbx_context_t *ctx)
{
    assert(NULL != ctx);
    return ctx->cpu_speed;
}

// -----------------------------------------------------------------------------
static void simulate_starting_state(gbx_context_t *ctx)
{
    cpu_registers_t *r = &ctx->reg;
    uint8_t NR52 = 0xF1;

    r->af = 0x01B0;
    r->bc = 0x0013;
    r->de = 0x00D8;
    r->hl = 0x014D;
    r->sp = 0xFFFE;
    r->pc = 0x0100;

    // contents vary between models, and can be used to detect system type
    switch (ctx->system) {
    case SYSTEM_SGB:
        NR52 = 0xF0;
        break;
    case SYSTEM_SGB2:
    case SYSTEM_GBP:
        r->a = 0xFF;
        break;
    case SYSTEM_GBA:
        r->a = 0x11;
        r->b = 0x01;
        break;
    case SYSTEM_CGB:
        r->a = 0x11;
        break;
    }

    // initial register values taken from pan docs and gameboy cpu manual
    gbx_write_byte(ctx, 0xFF05, 0x00); // TIMA
    gbx_write_byte(ctx, 0xFF06, 0x00); // TMA
    gbx_write_byte(ctx, 0xFF07, 0x00); // TAC
    gbx_write_byte(ctx, 0xFF10, 0x80); // NR10
    gbx_write_byte(ctx, 0xFF11, 0xBF); // NR11
    gbx_write_byte(ctx, 0xFF12, 0xF3); // NR12
    gbx_write_byte(ctx, 0xFF14, 0xBF); // NR14
    gbx_write_byte(ctx, 0xFF16, 0x3F); // NR21
    gbx_write_byte(ctx, 0xFF17, 0x00); // NR22
    gbx_write_byte(ctx, 0xFF19, 0xBF); // NR24
    gbx_write_byte(ctx, 0xFF1A, 0x7F); // NR30
    gbx_write_byte(ctx, 0xFF1B, 0xFF); // NR31
    gbx_write_byte(ctx, 0xFF1C, 0x9F); // NR32
    gbx_write_byte(ctx, 0xFF1E, 0xBF); // NR33
    gbx_write_byte(ctx, 0xFF20, 0xFF); // NR41
    gbx_write_byte(ctx, 0xFF21, 0x00); // NR42
    gbx_write_byte(ctx, 0xFF22, 0x00); // NR43
    gbx_write_byte(ctx, 0xFF23, 0xBF); // NR30
    gbx_write_byte(ctx, 0xFF24, 0x77); // NR50
    gbx_write_byte(ctx, 0xFF25, 0xF3); // NR51
    gbx_write_byte(ctx, 0xFF26, NR52); // NR52
    gbx_write_byte(ctx, 0xFF40, 0x91); // LCDC
    gbx_write_byte(ctx, 0xFF42, 0x00); // SCY
    gbx_write_byte(ctx, 0xFF43, 0x00); // SCX
    gbx_write_byte(ctx, 0xFF45, 0x00); // LYC
    gbx_write_byte(ctx, 0xFF47, 0xFC); // BGP
    gbx_write_byte(ctx, 0xFF48, 0xFF); // OBP0
    gbx_write_byte(ctx, 0xFF49, 0xFF); // OBP1
    gbx_write_byte(ctx, 0xFF4A, 0x00); // WY
    gbx_write_byte(ctx, 0xFF4B, 0x00); // WX
    gbx_write_byte(ctx, 0xFFFF, 0x00); // IE
}

// -----------------------------------------------------------------------------
void gbx_power_on(gbx_context_t *ctx)
{
    if (ctx->bios_enabled) {
        // if bios is available, start execution at first address in memory
        ctx->reg.pc = 0x0000;
    }
    else {
        // if bios is not available, put system in a know post-bios state
        simulate_starting_state(ctx);
    }
}

// -----------------------------------------------------------------------------
void gbx_req_interrupt(gbx_context_t *ctx, int interrupt)
{
    assert(NULL != ctx);
    assert(interrupt >= INT_VBLANK && interrupt <= INT_JOYPAD);

    // this will transfer to int_flags after the next instruction is executed
    // to simulate non-zero interrupt latency. probably not correct timing...
    ctx->int_flags_delay |= interrupt;
}

