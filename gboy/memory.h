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

#ifndef GBOY_MEMORY__H
#define GBOY_MEMORY__H

uint8_t gbx_read_byte(gbx_context_t *ctx, uint16_t addr);
void    gbx_write_byte(gbx_context_t *ctx, uint16_t addr, uint8_t data);

// -----------------------------------------------------------------------------
INLINE uint16_t gbx_read_word(gbx_context_t *ctx, uint16_t addr)
{
    return gbx_read_byte(ctx, addr) | gbx_read_byte(ctx, addr + 1) << 8;
}

// -----------------------------------------------------------------------------
INLINE void gbx_write_word(gbx_context_t *ctx, uint16_t addr, uint16_t data)
{
    gbx_write_byte(ctx, addr, data & 0xFF);
    gbx_write_byte(ctx, addr + 1, data >> 8);
}

// -----------------------------------------------------------------------------
INLINE uint8_t gbx_next_byte(gbx_context_t *ctx)
{
    ++ctx->bytes_read;
    return gbx_read_byte(ctx, ctx->next_pc++);
}

// -----------------------------------------------------------------------------
INLINE uint16_t gbx_next_word(gbx_context_t *ctx)
{
    uint16_t data = gbx_read_byte(ctx, ctx->next_pc++);
    data |= (gbx_read_byte(ctx, ctx->next_pc++) << 8);
    ctx->bytes_read += 2;
    return data;
}

#endif // GBOY_MEMORY__H

