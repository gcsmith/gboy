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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include "cmdline.h"
#include "gbx.h"

#define CMDLINE_VERSION         1000
#define CMDLINE_SYSTEM_DMG      1001
#define CMDLINE_SYSTEM_CGB      1002
#define CMDLINE_SYSTEM_SGB      1003
#define CMDLINE_SYSTEM_SGB2     1004
#define CMDLINE_SYSTEM_GBA      1005
#define CMDLINE_LOG_SERIAL      1006
#define CMDLINE_NO_SOUND        1007

const char *gboy_desc   = "gboy - a portable gameboy emulator";
const char *gboy_usage  = "usage: gboy [options] [file]";

// ----------------------------------------------------------------------------
void cmdline_display_usage(void)
{
    log_info("%s\n%s\n\n", gboy_desc, gboy_usage);
    log_info("Options:\n"
        "  -b, --bios-dir=PATH      specify where bios files are located\n"
        "  -d, --debugger           enable debugging interface\n"
        "  -f, --fullscreen         run in fullscreen mode\n"
        "      --log-serial=PATH    log serial output to the specified file\n"
        "      --no-sound           disable sound playback\n"
        "  -r, --rom=PATH           path to rom file\n"
        "  -s, --scale=INT          scale screen resolution\n"
        "  -S, --stretch            stretch image to fill screen\n"
        "      --system-dmg         force system type to original game boy\n"
        "      --system-cgb         force system type to game boy color\n"
        "      --system-sgb         force system type to super game boy\n"
        "      --system-sgb2        force system type to super game boy 2\n"
        "      --system-gba         force system type to game boy advance\n"
        "  -u, --unlock             unlock cpu throttling (no speed limit)\n"
        "  -v, --vsync              enable vertical sync\n"
        "  -h, --help               display this usage message\n"
        "      --version            display program version\n");
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
void cmdline_display_version(void)
{
    log_info("gboy %s\n", GBOY_ID_STR);
    log_info("compiled %s %s\n", __DATE__, __TIME__);
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
int cmdline_parse(int argc, char *argv[], cmdargs_t *args)
{
    static const char *s_opts = "b:dfr:s:Suvh?";
    static const struct option l_opts[] = {
        { "bios-dir",       required_argument,  NULL, 'b' },
        { "debugger",       no_argument,        NULL, 'd' },
        { "fullscreen",     no_argument,        NULL, 'f' },
        { "log-serial",     required_argument,  NULL, CMDLINE_LOG_SERIAL },
        { "no-sound",       no_argument,        NULL, CMDLINE_NO_SOUND },
        { "rom",            required_argument,  NULL, 'r' },
        { "scale",          required_argument,  NULL, 's' },
        { "stretch",        no_argument,        NULL, 'S' },
        { "system-dmg",     no_argument,        NULL, CMDLINE_SYSTEM_DMG },
        { "system-cgb",     no_argument,        NULL, CMDLINE_SYSTEM_CGB },
        { "system-sgb",     no_argument,        NULL, CMDLINE_SYSTEM_SGB },
        { "system-sgb2",    no_argument,        NULL, CMDLINE_SYSTEM_SGB2 },
        { "system-gba",     no_argument,        NULL, CMDLINE_SYSTEM_GBA },
        { "unlock",         no_argument,        NULL, 'u' },
        { "vsync",          no_argument,        NULL, 'v' },
        { "help",           no_argument,        NULL, 'h' },
        { "version",        no_argument,        NULL, CMDLINE_VERSION },
        { NULL,             no_argument,        NULL, 0 }
    };
    int opt, index = 0, scale = 1;

    // set some reasonable defaults
    args->system = SYSTEM_AUTO;
    args->debugger = 0;
    args->fullscreen = 0;
    args->stretch = 0;
    args->unlock = 0;
    args->vsync = 0;
    args->enable_sound = 1;
    args->rom_path = NULL;
    args->bios_path = NULL;
    args->serial_path = NULL;

    while (-1 != (opt = getopt_long(argc, argv, s_opts, l_opts, &index))) {
        switch (opt) {
        case 'b':
            args->bios_path = strdup(optarg);
            break;
        case 'd':
            args->debugger = 1;
            break;
        case 'r':
            args->rom_path = strdup(optarg);
            break;
        case 's':
            scale = strtol(optarg, NULL, 0);
            break;
        case 'S':
            args->stretch = 1;
            break;
        case 'f':
            args->fullscreen = 1;
            break;
        case 'u':
            args->unlock = 1;
            break;
        case 'v':
            args->vsync = 1;
            break;
        case CMDLINE_SYSTEM_DMG:
            args->system = SYSTEM_DMG;
            break;
        case CMDLINE_SYSTEM_CGB:
            args->system = SYSTEM_CGB;
            break;
        case CMDLINE_SYSTEM_SGB:
            args->system = SYSTEM_SGB;
            break;
        case CMDLINE_SYSTEM_SGB2:
            args->system = SYSTEM_SGB2;
            break;
        case CMDLINE_SYSTEM_GBA:
            args->system = SYSTEM_GBA;
            break;
        case CMDLINE_VERSION:
            cmdline_display_version();
            break;
        case CMDLINE_LOG_SERIAL:
            args->serial_path = strdup(optarg);
            break;
        case CMDLINE_NO_SOUND:
            args->enable_sound = 0;
            break;
        case 'h':
        case '?':
            cmdline_display_usage();
            break;
        default:
            break;
        }
    }

    // check for non-option arguments. if -r isn't specified, treat as romfile
    if (optind < argc) {
        if (args->rom_path || (argc - optind) > 1) {
            log_err("trailing arguments (%d)\n", argc - optind);
            return -1;
        }
        args->rom_path = strdup(argv[optind]);
    }

    if (!args->rom_path) {
        log_err("no rom file specified\n");
        return -1;
    }

    if (scale <= 0 || scale > 10) {
        log_err("invalid scale factor specified (must be 0-20)\n");
        return -1;
    }
    args->width  = GBX_LCD_XRES * scale;
    args->height = GBX_LCD_YRES * scale;

    return 0;
}

// ----------------------------------------------------------------------------
void cmdline_destroy(cmdargs_t *args)
{
    assert(NULL != args);
    SAFE_FREE(args->serial_path);
    SAFE_FREE(args->bios_path);
    SAFE_FREE(args->rom_path);
}

