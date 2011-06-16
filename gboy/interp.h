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

#define PUSH(x) do { gbx_write_byte(ctx, --rSP, x >> 8);                    \
                     gbx_write_byte(ctx, --rSP, x & 0xFF); } while (0)

#define POP(x)  do { x  = gbx_read_byte(ctx, rSP++);                        \
                     x |= gbx_read_byte(ctx, rSP++) << 8; } while (0)

#endif // GBOY_INTERP__H

