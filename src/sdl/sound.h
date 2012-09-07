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

#ifndef GBOY_SOUND__H
#define GBOY_SOUND__H

#include "common.h"

typedef void sound_t;

#ifdef __cplusplus
extern "C" {
#endif

sound_t *sound_init(int sample_rate, int buff_size);
void     sound_set_freq(sound_t *snd, int hz);
void     sound_write(sound_t *snd, int cycles, uint16_t addr, uint8_t value);
uint8_t  sound_read(sound_t *snd, int cycles, uint16_t addr);
void     sound_render(sound_t *snd, int cycles);
void     sound_shutdown(sound_t *snd);

#ifdef __cplusplus
}
#endif

#endif // GBOY_SOUND__H

