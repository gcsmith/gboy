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

#ifndef GBOY_INTERP__H
#define GBOY_INTERP__H

#define OP_FUNC INLINE void

#define reg8    ctx->reg.vb
#define reg16   ctx->reg.vw

#define rA      ctx->reg.a
#define rF      ctx->reg.f
#define rB      ctx->reg.b
#define rC      ctx->reg.c
#define rD      ctx->reg.d
#define rE      ctx->reg.e
#define rH      ctx->reg.h
#define rL      ctx->reg.l

#define rAF     ctx->reg.af
#define rBC     ctx->reg.bc
#define rDE     ctx->reg.de
#define rHL     ctx->reg.hl
#define rSP     ctx->reg.sp
#define rPC     ctx->reg.pc
#define rNextPC ctx->next_pc
#define rExtCyc ctx->cycle_delta

#define PUSH(x) do { gbx_write_byte(ctx, --rSP, x >> 8);                    \
                     gbx_write_byte(ctx, --rSP, x & 0xFF); } while (0)

#define POP(x)  do { x  = gbx_read_byte(ctx, rSP++);                        \
                     x |= gbx_read_byte(ctx, rSP++) << 8; } while (0)

#define CONDITIONAL_JP(exp, nn, c)                                          \
    do { if (exp) { rNextPC = nn; rExtCyc = c; } } while(0)

#define CONDITIONAL_JR(exp, n, c)                                           \
    do { if (exp) { rNextPC += n; rExtCyc = c; } } while(0)

#define CONDITIONAL_RET(exp, c)                                             \
    do { if (exp) { POP(rNextPC); rExtCyc = c; } } while (0)

#define CONDITIONAL_CALL(exp, nn, c)                                        \
    do { if (exp) { PUSH(rNextPC); rNextPC = nn; rExtCyc = c; } } while (0)

#define Z_TST(x)            ((x) ? 0 : FLAG_Z)
#define H_TST(a, b, r)      (((a) ^ (b) ^ (r)) &  0x10 ? FLAG_H : 0)
#define H_TST_16(a, b, r)   (((a) ^ (b) ^ (r)) & 0x1000 ? FLAG_H : 0)
#define H_TST_N(x)          (((x) & 0xF) == 0xF ? FLAG_H : 0)
#define H_TST_P(x)          ((x) & 0xF ? 0 : FLAG_H)
#define C_TST(a, b, r)      (((a) ^ (b) ^ (r)) & 0x100 ? FLAG_C : 0)
#define C_TST_N(x)          (((x) < 0) ? FLAG_C : 0)
#define C_TST_P(x)          (((x) > 0xFF) ? FLAG_C : 0)
#define C_TST_P16(x)        (((x) > 0xFFFF) ? FLAG_C : 0)

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

#endif // GBOY_INTERP__H

