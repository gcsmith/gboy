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

#ifndef GBOY_CMDLINE__H
#define GBOY_CMDLINE__H

typedef struct cmdargs {
    int system;         // type of system to emulate
    int debugger;       // enable debugging interface
    int fullscreen;     // start in fullscreen mode
    int width;          // display width
    int height;         // display height
    int vsync;          // enable vertical sync
    int stretch;        // stretch image to fill screen
    int unlock;         // unlock cpu throttling
    int enable_sound;   // enable or disable sound playback
    char *rom_path;     // path to rom image
    char *bios_path;    // path to bios directory
    char *serial_path;  // path to serial log file
} cmdargs_t;

int cmdline_parse(int argc, char *argv[], cmdargs_t *args);
void cmdline_destroy(cmdargs_t *args);
void cmdline_display_usage(void);
void cmdline_display_version(void);

#endif // GBOY_CMDLINE__H

