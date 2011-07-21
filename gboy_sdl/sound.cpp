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

#include "Gb_Apu.h"
#include "Multi_Buffer.h"
#include "Sound_Queue.h"
#include "gbx.h"
#include "sound.h"

struct blargg_sound {
    Gb_Apu apu;
    Sound_Queue queue;
    Stereo_Buffer buf;
    blip_sample_t *out_buf;
    int out_size;
    int sample_rate;
};

// ----------------------------------------------------------------------------
sound_t *sound_init(int sample_rate, int buff_size)
{
    blargg_sound *psnd = new blargg_sound;
    psnd->out_buf = new blip_sample_t[buff_size];
    psnd->out_size = buff_size;
    psnd->sample_rate = sample_rate;

    const char *err;
    if (NULL != (err = psnd->buf.set_sample_rate(sample_rate)))
        log_err("error in APU: %s\n", err);

    psnd->apu.output(psnd->buf.center(), psnd->buf.left(), psnd->buf.right());

    if (NULL != (err = psnd->queue.start(sample_rate, 2)))
        log_err("error in APU: %s\n", err);

    return (sound_t *)psnd;
}

// ----------------------------------------------------------------------------
void sound_set_freq(sound_t *snd, int hz)
{
    blargg_sound *psnd = (blargg_sound *)snd;
    psnd->buf.clock_rate(hz);
}

// ----------------------------------------------------------------------------
void sound_write(sound_t *snd, int cycle, uint16_t addr, uint8_t value)
{
    blargg_sound *psnd = (blargg_sound *)snd;
    if (addr < psnd->apu.start_addr || addr > psnd->apu.end_addr) {
        log_err("APU write error: address %04X out of range\n", addr);
        return;
    }

    psnd->apu.write_register(cycle, addr, value);
}

// ----------------------------------------------------------------------------
uint8_t sound_read(sound_t *snd, int cycle, uint16_t addr)
{
    blargg_sound *psnd = (blargg_sound *)snd;
    if (addr < psnd->apu.start_addr || addr > psnd->apu.end_addr) {
        log_err("APU read error: address %04X out of range\n", addr);
        return 0xFF;
    }

    return psnd->apu.read_register(cycle, addr);
}

// ----------------------------------------------------------------------------
void sound_render(sound_t *snd, int cycle)
{
    blargg_sound *psnd = (blargg_sound *)snd;

    bool stereo = psnd->apu.end_frame(cycle);
    psnd->buf.end_frame(cycle, stereo);

    while (psnd->buf.samples_avail() >= psnd->out_size) {
        size_t count = psnd->buf.read_samples(psnd->out_buf, psnd->out_size);
        psnd->queue.write(psnd->out_buf, count);

        log_spew("APU cycle %d processed %d samples, %d available\n",
                 cycle, psnd->out_size, psnd->buf.samples_avail());
    }
}

// ----------------------------------------------------------------------------
void sound_shutdown(sound_t *snd)
{
    blargg_sound *psnd = (blargg_sound *)snd;
    if (snd) {
        delete[] psnd->out_buf;
        delete psnd;
    }
}

