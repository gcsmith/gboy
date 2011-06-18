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

#include <assert.h>
#include "gbx.h"
#include "interp.h"
#include "memory.h"
#include "ports.h"

// -----------------------------------------------------------------------------
// decimal adjust register A
OP_FUNC op_daa(gbx_context_t *ctx)
{
    rAF = gbx_daa_lut[rA | (rF & (FLAG_C | FLAG_H | FLAG_N)) << 4];
}

// -----------------------------------------------------------------------------
// complement A register
OP_FUNC op_cpl(gbx_context_t *ctx)
{
    rA = ~rA;
    rF |= (FLAG_N | FLAG_H);
}

// -----------------------------------------------------------------------------
// complement carry flag
OP_FUNC op_ccf(gbx_context_t *ctx)
{
    rF = (rF ^ FLAG_C) & (FLAG_Z | FLAG_C);
}

// -----------------------------------------------------------------------------
// set carry flag
OP_FUNC op_scf(gbx_context_t *ctx)
{
    rF = (rF & FLAG_Z) | FLAG_C;
}

// -----------------------------------------------------------------------------
// no operation
OP_FUNC op_nop(gbx_context_t *ctx)
{
}

// -----------------------------------------------------------------------------
// halt system - power down cpu until interrupt
OP_FUNC op_halt(gbx_context_t *ctx)
{
    ctx->exec_flags |= EXEC_HALT;
}

// -----------------------------------------------------------------------------
// stop system - halt cpu and lcd display until button press
OP_FUNC op_stop(gbx_context_t *ctx)
{
    ctx->exec_flags |= EXEC_STOP;
}

// -----------------------------------------------------------------------------
// disable interrupts, effective after the instruction following DI
OP_FUNC op_di(gbx_context_t *ctx)
{
    ctx->di_delay = 1;
}

// -----------------------------------------------------------------------------
// enable interrupts, effective after the instruction following EI
OP_FUNC op_ei(gbx_context_t *ctx)
{
    ctx->ei_delay = 1;
}

// -----------------------------------------------------------------------------
// rotate A register left, store old bit 7 in carry flag
OP_FUNC op_rlca(gbx_context_t *ctx)
{
    rF = (rA & 0x80) ? FLAG_C : 0;
    rA = _rotl8(rA, 1);
}

// -----------------------------------------------------------------------------
// rotate 8-bit register left, store old bit 7 in carry flag
OP_FUNC op_rlc(gbx_context_t *ctx, int rd)
{
    rF = (reg8[rd] & 0x80) ? FLAG_C : 0;
    reg8[rd] = _rotl8(reg8[rd], 1);
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// rotate (HL) left, store old bit 7 in carry flag
OP_FUNC op_rlci(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (mem & 0x80) ? FLAG_C : 0;
    mem = _rotl8(mem, 1);
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// rotate A register left through carry flag
OP_FUNC op_rla(gbx_context_t *ctx)
{
    int carry = (rF & FLAG_C) ? 1 : 0;
    rF = (rA & 0x80) ? FLAG_C : 0;
    rA = (rA << 1) | carry;
}

// -----------------------------------------------------------------------------
// rotate 8-bit register left through carry flag
OP_FUNC op_rl(gbx_context_t *ctx, int rd)
{
    int carry = (rF & FLAG_C) ? 1 : 0;
    rF = (reg8[rd] & 0x80) ? FLAG_C : 0;
    reg8[rd] = (reg8[rd] << 1) | carry;
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// rotate (HL) left through carry flag
OP_FUNC op_rli(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int carry = (rF & FLAG_C) ? 1 : 0;
    rF = (mem & 0x80) ? FLAG_C : 0;
    mem = (mem << 1) | carry;
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// rotate A register right, store old bit 0 in carry flag
OP_FUNC op_rrca(gbx_context_t *ctx)
{
    rF = (rA & 1) ? FLAG_C : 0;
    rA = _rotr8(rA, 1);
}

// -----------------------------------------------------------------------------
// rotate 8-bit register right, store old bit 0 in carry flag
OP_FUNC op_rrc(gbx_context_t *ctx, int rd)
{
    rF = (reg8[rd] & 1) ? FLAG_C : 0;
    reg8[rd] = _rotr8(reg8[rd], 1);
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// rotate (HL) right, store old bit 0 in carry flag
OP_FUNC op_rrci(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (mem & 1) ? FLAG_C : 0;
    mem = _rotr8(mem, 1);
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// rotate A register right through carry flag
OP_FUNC op_rra(gbx_context_t *ctx)
{
    int carry = (rF & FLAG_C) ? 0x80 : 0;
    rF = (rA & 1) ? FLAG_C : 0;
    rA = (rA >> 1) | carry;
}

// -----------------------------------------------------------------------------
// rotate 8-bit register right through carry flag
OP_FUNC op_rr(gbx_context_t *ctx, int rd)
{
    int carry = (rF & FLAG_C) ? 0x80 : 0;
    rF = (reg8[rd] & 1) ? FLAG_C : 0;
    reg8[rd] = (reg8[rd] >> 1) | carry;
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// rotate (HL) right through carry flag
OP_FUNC op_rri(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int carry = (rF & FLAG_C) ? 0x80 : 0;
    rF = (mem & 1) ? FLAG_C : 0;
    mem = (mem >> 1) | carry;
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// arithmetic shift 8-bit register left into carry, LSB set to 0
OP_FUNC op_sla(gbx_context_t *ctx, int rd)
{
    rF = (reg8[rd] & 0x80) ? FLAG_C : 0;
    reg8[rd] <<= 1;
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// arithmetic shift (HL) left into carry, LSB set to 0
OP_FUNC op_slai(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (mem & 0x80) ? FLAG_C : 0;
    mem <<= 1;
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// arithmetic shift 8-bit register right into carry, MSB holds its value
OP_FUNC op_sra(gbx_context_t *ctx, int rd)
{
    rF = (reg8[rd] & 1) ? FLAG_C : 0;
    reg8[rd] = (reg8[rd] >> 1) | (reg8[rd] & 0x80);
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// arithmetic shift (HL) right into carry, MSB holds its value
OP_FUNC op_srai(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (mem & 1) ? FLAG_C : 0;
    mem = (mem >> 1) | (mem & 0x80);
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// swap upper and lower nibbles of 8-bit register
OP_FUNC op_swap(gbx_context_t *ctx, int rd)
{
    reg8[rd] = ((reg8[rd] & 0xF0) >> 4) | ((reg8[rd] & 0x0F) << 4);
    rF = Z_TST(reg8[rd]);
}

// -----------------------------------------------------------------------------
// swap upper and lower nibbles of (HL)
OP_FUNC op_swapi(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    mem = ((mem & 0xF0) >> 4) | ((mem & 0x0F) << 4);
    rF = Z_TST(mem);
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// logic shift 8-bit register right into carry, MSB set to 0
OP_FUNC op_srl(gbx_context_t *ctx, int rd)
{
    rF = (reg8[rd] & 1) ? FLAG_C : 0;
    reg8[rd] >>= 1;
    if (!reg8[rd]) rF |= FLAG_Z;
}

// -----------------------------------------------------------------------------
// logic shift (HL) right into carry, MSB set to 0
OP_FUNC op_srli(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (mem & 1) ? FLAG_C : 0;
    mem >>= 1;
    if (!mem) rF |= FLAG_Z;
    gbx_write_byte(ctx, rHL, mem);
}

// -----------------------------------------------------------------------------
// test bit b of 8-bit register
OP_FUNC op_bit(gbx_context_t *ctx, int b, int rd)
{
    rF = (rF & FLAG_C) | FLAG_H | Z_TST(reg8[rd] & (1 << b));
}

// -----------------------------------------------------------------------------
// test bit b of (HL)
OP_FUNC op_biti(gbx_context_t *ctx, int b)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    rF = (rF & FLAG_C) | FLAG_H | Z_TST(mem & (1 << b));
}

// -----------------------------------------------------------------------------
// reset bit b of 8-bit register to 0
OP_FUNC op_res(gbx_context_t *ctx, int b, int rd)
{
    reg8[rd] &= ~(1 << b);
}

// -----------------------------------------------------------------------------
// reset bit b of (HL) to 0
OP_FUNC op_resi(gbx_context_t *ctx, int b)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    gbx_write_byte(ctx, rHL, mem & ~(1 << b));
}

// -----------------------------------------------------------------------------
// set bit b of 8-bit register to 1
OP_FUNC op_set(gbx_context_t *ctx, int b, int rd)
{
    reg8[rd] |= (1 << b);
}

// -----------------------------------------------------------------------------
// set bit b of (HL) to 1
OP_FUNC op_seti(gbx_context_t *ctx, int b)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    gbx_write_byte(ctx, rHL, mem | (1 << b));
}

// -----------------------------------------------------------------------------
// load register A into address $FF00 + register C
OP_FUNC op_ldac(gbx_context_t *ctx)
{
    gbx_write_byte(ctx, 0xFF00 + rC, rA);
}

// -----------------------------------------------------------------------------
// load value at address $FF00 + register C into register A
OP_FUNC op_ldca(gbx_context_t *ctx)
{
    rA = gbx_read_byte(ctx, 0xFF00 + rC);
}

// -----------------------------------------------------------------------------
// load value at address in HL into 8-bit register A, decrement HL
OP_FUNC op_ldd_ld(gbx_context_t *ctx)
{
    rA = gbx_read_byte(ctx, rHL--);
}

// -----------------------------------------------------------------------------
// load 8-bit register A into value at address in HL, decrement HL
OP_FUNC op_ldd_st(gbx_context_t *ctx)
{
    gbx_write_byte(ctx, rHL--, rA);
}

// -----------------------------------------------------------------------------
// load value at address in HL into 8-bit register A, increment HL
OP_FUNC op_ldi_ld(gbx_context_t *ctx)
{
    rA = gbx_read_byte(ctx, rHL++);
}

// -----------------------------------------------------------------------------
// load 8-bit register A into value at address in HL, increment HL
OP_FUNC op_ldi_st(gbx_context_t *ctx)
{
    gbx_write_byte(ctx, rHL++, rA);
}

// -----------------------------------------------------------------------------
// load value at address $FF00 + n into 8-bit register A
OP_FUNC op_ldh_ld(gbx_context_t *ctx, uint8_t n)
{
    rA = gbx_read_byte(ctx, 0xFF00 + n);
}

// -----------------------------------------------------------------------------
// load 8-bit register A into value at address $FF00 + n
OP_FUNC op_ldh_st(gbx_context_t *ctx, uint8_t n)
{
    gbx_write_byte(ctx, 0xFF00 + n, rA);
}

// -----------------------------------------------------------------------------
// load 16-bit register HL into stack pointer SP
OP_FUNC op_ldsp_ld(gbx_context_t *ctx)
{
    rSP = rHL;
}

// -----------------------------------------------------------------------------
// load stack pointer SP into value at immediate address
OP_FUNC op_ldsp_st(gbx_context_t *ctx, uint16_t nn)
{
    gbx_write_word(ctx, nn, rSP);
}

// -----------------------------------------------------------------------------
// load effective address SP + n into 16-bit register HL
OP_FUNC op_ldhl(gbx_context_t *ctx, int8_t n)
{
    int result = rSP + n;
    rF = C_TST(rSP, n, result) | H_TST(rSP, n, result);
    rHL = result & 0xFFFF;
}

// -----------------------------------------------------------------------------
// load 8-bit register into 8-bit register
OP_FUNC op_ld_r8r8(gbx_context_t *ctx, int rs, int rd)
{
    reg8[rd] = reg8[rs];
}

// -----------------------------------------------------------------------------
OP_FUNC op_ld_r8m8(gbx_context_t *ctx, int rs, uint16_t addr)
{
    gbx_write_byte(ctx, addr, reg8[rs]);
}

// -----------------------------------------------------------------------------
// load 8-bit register into byte at address in 16-bit register
OP_FUNC op_ld_r8ir16(gbx_context_t *ctx, int rs, int rd)
{
    gbx_write_byte(ctx, reg16[rd], reg8[rs]);
}

// -----------------------------------------------------------------------------
// load 8-bit immediate value into 8-bit register
OP_FUNC op_ld_n8r8(gbx_context_t *ctx, uint8_t n, int rd)
{
    reg8[rd] = n;
}

// -----------------------------------------------------------------------------
OP_FUNC op_ld_n8ir16(gbx_context_t *ctx, uint8_t n, int rd)
{
    gbx_write_byte(ctx, reg16[rd], n);
}

// -----------------------------------------------------------------------------
OP_FUNC op_ld_m8r8(gbx_context_t *ctx, uint16_t addr, int rd)
{
    reg8[rd] = gbx_read_byte(ctx, addr);
}

// -----------------------------------------------------------------------------
// load 16-bit immediate value into 16-bit register pair
OP_FUNC op_ld_n16r16(gbx_context_t *ctx, uint16_t nn, int rd)
{
    reg16[rd] = nn;
}

// -----------------------------------------------------------------------------
// load byte at address in 16-bit register into 8-bit register
OP_FUNC op_ld_ir16r8(gbx_context_t *ctx, int rs, int rd)
{
    reg8[rd] = gbx_read_byte(ctx, reg16[rs]);
}

// -----------------------------------------------------------------------------
// push 16-bit register into stack after decrementing SP by 2
OP_FUNC op_push(gbx_context_t *ctx, int rs)
{
    PUSH(reg16[rs]);
}

// -----------------------------------------------------------------------------
// load value from stack into 16-bit register, increment SP by 2
OP_FUNC op_pop(gbx_context_t *ctx, int rd)
{
    POP(reg16[rd]);
}

// -----------------------------------------------------------------------------
// load value from stack into AF with flag mask, increment SP by 2
OP_FUNC op_popaf(gbx_context_t *ctx)
{
    POP(rAF);
    rF &= 0xF0;
}

// -----------------------------------------------------------------------------
OP_FUNC op_cp(gbx_context_t *ctx, int rs)
{
    int r = rA - reg8[rs];
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, reg8[rs], r);
}

// -----------------------------------------------------------------------------
OP_FUNC op_cpn(gbx_context_t *ctx, uint8_t n)
{
    int r = rA - n;
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, n, r);
}

// -----------------------------------------------------------------------------
OP_FUNC op_cpi(gbx_context_t *ctx, int rs)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int r = rA - mem;
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, mem, r);
}

// -----------------------------------------------------------------------------
OP_FUNC op_and(gbx_context_t *ctx, int rs)
{
    rA &= reg8[rs];
    rF = rA ? FLAG_H : (FLAG_Z | FLAG_H);
}

// -----------------------------------------------------------------------------
OP_FUNC op_andn(gbx_context_t *ctx, uint8_t n)
{
    rA &= n;
    rF = rA ? FLAG_H : (FLAG_Z | FLAG_H);
}

// -----------------------------------------------------------------------------
OP_FUNC op_andi(gbx_context_t *ctx)
{
    rA &= gbx_read_byte(ctx, rHL);
    rF = rA ? FLAG_H : (FLAG_Z | FLAG_H);
}

// -----------------------------------------------------------------------------
OP_FUNC op_or(gbx_context_t *ctx, int rs)
{
    rA |= reg8[rs];
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_orn(gbx_context_t *ctx, uint8_t n)
{
    rA |= n;
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_ori(gbx_context_t *ctx)
{
    rA |= gbx_read_byte(ctx, rHL);
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_xor(gbx_context_t *ctx, int rs)
{
    rA ^= reg8[rs];
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_xorn(gbx_context_t *ctx, uint8_t n)
{
    rA ^= n;
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_xori(gbx_context_t *ctx)
{
    rA ^= gbx_read_byte(ctx, rHL);
    rF = Z_TST(rA);
}

// -----------------------------------------------------------------------------
OP_FUNC op_addsp(gbx_context_t *ctx, int8_t n)
{
    int result = rSP + n;
    rF = C_TST(rSP, n, result) | H_TST(rSP, n, result);
    rSP = result & 0xFFFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_add(gbx_context_t *ctx, int rs)
{
    int r = (int)rA + reg8[rs];
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, reg8[rs], r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_addn(gbx_context_t *ctx, uint8_t n)
{
    int r = (int)rA + n;
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, n, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_addi(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int r = (int)rA + mem;
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, mem, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_add_r16(gbx_context_t *ctx, int rs)
{
    int r = (int)rHL + reg16[rs];
    rF = (rF & FLAG_Z) | C_TST_P16(r) | H_TST_16(rHL, reg16[rs], r);
    rHL = r & 0xFFFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_adc(gbx_context_t *ctx, int rs)
{
    int r = (int)rA + reg8[rs] + (rF & FLAG_C ? 1 : 0);
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, reg8[rs], r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_adcn(gbx_context_t *ctx, uint8_t n)
{
    int r = (int)rA + n + (rF & FLAG_C ? 1 : 0);
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, n, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_adci(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int r = (int)rA + mem + (rF & FLAG_C ? 1 : 0);
    rF = Z_TST(r & 0xFF) | C_TST_P(r) | H_TST(rA, mem, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_sub(gbx_context_t *ctx, int rs)
{
    int r = (int)rA - reg8[rs];
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, reg8[rs], r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_subn(gbx_context_t *ctx, uint8_t n)
{
    int r = (int)rA - n;
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, n, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_subi(gbx_context_t *ctx)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int r = (int)rA - mem;
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, mem, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_sbc(gbx_context_t *ctx, int rs)
{
    int r = (int)rA - reg8[rs] - (rF & FLAG_C ? 1 : 0);
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, reg8[rs], r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_sbcn(gbx_context_t *ctx, uint8_t n)
{
    int r = (int)rA - n - (rF & FLAG_C ? 1 : 0);
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, n, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_sbci(gbx_context_t *ctx, int rs)
{
    uint8_t mem = gbx_read_byte(ctx, rHL);
    int r = (int)rA - mem - (rF & FLAG_C ? 1 : 0);
    rF = FLAG_N | Z_TST(r & 0xFF) | C_TST_N(r) | H_TST(rA, mem, r);
    rA = r & 0xFF;
}

// -----------------------------------------------------------------------------
OP_FUNC op_incb(gbx_context_t *ctx, int rd)
{
    ++reg8[rd];
    rF = (rF & FLAG_C) | Z_TST(reg8[rd]) | H_TST_P(reg8[rd]);
}

// -----------------------------------------------------------------------------
OP_FUNC op_inci(gbx_context_t *ctx)
{
    uint8_t r = gbx_read_byte(ctx, rHL) + 1;
    rF = (rF & FLAG_C) | Z_TST(r) | H_TST_P(r);
    gbx_write_byte(ctx, rHL, r);
}

// -----------------------------------------------------------------------------
OP_FUNC op_incw(gbx_context_t *ctx, int rd)
{
    ++reg16[rd];
}

// -----------------------------------------------------------------------------
OP_FUNC op_decb(gbx_context_t *ctx, int rd)
{
    --reg8[rd];
    rF = (rF & FLAG_C) | FLAG_N | Z_TST(reg8[rd]) | H_TST_N(reg8[rd]);
}

// -----------------------------------------------------------------------------
OP_FUNC op_deci(gbx_context_t *ctx)
{
    uint8_t r = gbx_read_byte(ctx, rHL) - 1;
    rF = (rF & FLAG_C) | FLAG_N | Z_TST(r) | H_TST_N(r);
    gbx_write_byte(ctx, rHL, r);
}

// -----------------------------------------------------------------------------
OP_FUNC op_decw(gbx_context_t *ctx, int rd)
{
    --reg16[rd];
}

// -----------------------------------------------------------------------------
OP_FUNC op_jp(gbx_context_t *ctx, uint16_t nn)
{
    rNextPC = nn;
}

// -----------------------------------------------------------------------------
OP_FUNC op_jpnz(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_JP(!(rF & FLAG_Z), nn, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jpz(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_JP(rF & FLAG_Z, nn, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jpnc(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_JP(!(rF & FLAG_C), nn, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jpc(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_JP(rF & FLAG_C, nn, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jpi(gbx_context_t *ctx)
{
    rNextPC = rHL;
}

// -----------------------------------------------------------------------------
OP_FUNC op_jr(gbx_context_t *ctx, int8_t n)
{
    rNextPC += n;
}

// -----------------------------------------------------------------------------
OP_FUNC op_jrnz(gbx_context_t *ctx, int8_t n)
{
    CONDITIONAL_JR(!(rF & FLAG_Z), n, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jrz(gbx_context_t *ctx, int8_t n)
{
    CONDITIONAL_JR(rF & FLAG_Z, n, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jrnc(gbx_context_t *ctx, int8_t n)
{
    CONDITIONAL_JR(!(rF & FLAG_C), n, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_jrc(gbx_context_t *ctx, int8_t n)
{
    CONDITIONAL_JR(rF & FLAG_C, n, 1);
}

// -----------------------------------------------------------------------------
OP_FUNC op_call(gbx_context_t *ctx, uint16_t nn)
{
    PUSH(rNextPC);
    rNextPC = nn;
}

// -----------------------------------------------------------------------------
OP_FUNC op_callnz(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_CALL(!(rF & FLAG_Z), nn, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_callz(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_CALL(rF & FLAG_Z, nn, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_callnc(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_CALL(!(rF & FLAG_C), nn, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_callc(gbx_context_t *ctx, uint16_t nn)
{
    CONDITIONAL_CALL(rF & FLAG_C, nn, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_ret(gbx_context_t *ctx)
{
    POP(rNextPC);
}

// -----------------------------------------------------------------------------
OP_FUNC op_retnz(gbx_context_t *ctx)
{
    CONDITIONAL_RET(!(rF & FLAG_Z), 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_retz(gbx_context_t *ctx)
{
    CONDITIONAL_RET(rF & FLAG_Z, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_retnc(gbx_context_t *ctx)
{
    CONDITIONAL_RET(!(rF & FLAG_C), 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_retc(gbx_context_t *ctx)
{
    CONDITIONAL_RET(rF & FLAG_C, 3);
}

// -----------------------------------------------------------------------------
OP_FUNC op_reti(gbx_context_t *ctx)
{
    POP(rNextPC);
    ctx->ime = IME_ENABLE;
}

// -----------------------------------------------------------------------------
OP_FUNC op_rst(gbx_context_t *ctx, uint8_t n)
{
    PUSH(rNextPC);
    rNextPC = n;
}

// -----------------------------------------------------------------------------
OP_FUNC op_reserved(gbx_context_t *ctx)
{
    log_err("attempting to execute invalid opcode\n");
}

// -----------------------------------------------------------------------------
INLINE void interrupt_vector(gbx_context_t *ctx, int src, uint16_t vector)
{
    // disable the master interrupt control and reset the corresponding IF bit
    ctx->ime = IME_DISABLE;
    ctx->int_flags &= ~src;

    // push the current PC to the stack and jump to the interrupt vector
    PUSH(rPC);
    rPC = vector;

    ctx->cycle_delta += 20;
}

// -----------------------------------------------------------------------------
INLINE void service_interrupts(gbx_context_t *ctx)
{
    int irqs = ctx->int_en & ctx->int_flags;

    // prioritize interrupts from VBLANK (highest) to JOYPAD (lowest)
    if (irqs & INT_VBLANK) {
        log_spew("servicing interrupt VBLANK\n");
        interrupt_vector(ctx, INT_VBLANK, INT_VEC_VBLANK);
    }
    else if (irqs & INT_LCDSTAT) {
        log_spew("servicing interrupt LCDSTAT\n");
        interrupt_vector(ctx, INT_LCDSTAT, INT_VEC_LCDSTAT);
    }
    else if (irqs & INT_TIMER) {
        log_spew("servicing interrupt TIMER\n");
        interrupt_vector(ctx, INT_TIMER, INT_VEC_TIMER);
    }
    else if (irqs & INT_SERIAL) {
        log_spew("servicing interrupt SERIAL\n");
        interrupt_vector(ctx, INT_SERIAL, INT_VEC_SERIAL);
    }
    else if (irqs & INT_JOYPAD) {
        log_spew("servicing interrupt JOYPAD\n");
        interrupt_vector(ctx, INT_JOYPAD, INT_VEC_JOYPAD);
    }
}

// -----------------------------------------------------------------------------
INLINE void timer_update_cycles(gbx_context_t *ctx, long cycles)
{
    // divider is always incremented, even if the main timer is disabled
    ctx->timer.div_ticks += cycles;
    if (ctx->timer.div_ticks >= 256) {
        ctx->timer.div_ticks -= 256;
        ctx->timer.div = (ctx->timer.div + 1) & 0xFF;
    }

    // update the main timer only if it's enabled
    if (!(ctx->timer.tac & TAC_ENABLED))
        return;

    ctx->timer.tima_ticks += cycles;
    if (ctx->timer.tima_ticks >= ctx->timer.tima_limit) {
        ctx->timer.tima_ticks -= ctx->timer.tima_limit;

        // load the timer modulo into the timer counter, set the interrupt
        if (++ctx->timer.tima >= 0x100) {
            ctx->timer.tima = ctx->timer.tma;
            gbx_req_interrupt(ctx, INT_TIMER);
        }
    }
}

// -----------------------------------------------------------------------------
INLINE void dma_update_cycles(gbx_context_t *ctx, long cycles)
{
    ctx->dma.cycle += cycles;
    if (ctx->dma.cycle >= 671) {
        ctx->dma.active = 0;
        ctx->dma.cycle = 0;
    }
    else if (ctx->dma.write_pos < 0x9F) {
        uint8_t data;
        uint16_t src = ctx->dma.src, dst = 0xFE00;
        int i, write_len = (ctx->dma.cycle - ctx->dma.write_cycle) >> 2;

        // try to write one byte every 4 clock cycles for ~160 us
        for (i = 0; i < write_len; ++i) {
            if (ctx->dma.write_pos > 0x9F)
                break;

            data = gbx_read_byte(ctx, src + ctx->dma.write_pos);
            gbx_write_byte(ctx, dst + ctx->dma.write_pos, data);
            ++ctx->dma.write_pos;
        }

        ctx->dma.write_cycle += (write_len << 2);
    }
}

// -----------------------------------------------------------------------------
INLINE void serial_update_cycles(gbx_context_t *ctx, long cycles)
{
    ctx->sc_ticks += cycles;
    if (ctx->sc_ticks >= 512) {
        gbx_req_interrupt(ctx, INT_SERIAL);
        ctx->sc_ticks = 0;
        ctx->sc_active = 0;
        ctx->sc &= ~SC_XFER_START;

        // if enabled, write the character data to the serial log file
        if (ctx->serial_log) {
            fprintf(ctx->serial_log, "%c", (char)ctx->sb);
            fflush(ctx->serial_log);
        }
    }
}

// -----------------------------------------------------------------------------
static void perform_cyclic_tasks(gbx_context_t *ctx)
{
    if (ctx->dma.active)
        dma_update_cycles(ctx, ctx->cycle_delta);

    if (ctx->sc_active)
        serial_update_cycles(ctx, ctx->cycle_delta);

    timer_update_cycles(ctx, ctx->cycle_delta);
    video_update_cycles(ctx, ctx->cycle_delta);

    // handle interrupts if any are enabled and pending, and IME is set
    if (ctx->ime == IME_ENABLE)
        service_interrupts(ctx);

    ctx->int_flags |= ctx->int_flags_delay;
    ctx->int_flags_delay = 0;

    if (ctx->ei_delay) {
        ctx->ei_delay = 0;
        ctx->ime = IME_ENABLE;
    }
    if (ctx->di_delay) {
        ctx->di_delay = 0;
        ctx->ime = IME_DISABLE;
    }

    ctx->cycles += ctx->cycle_delta;
}

#define HALT_CYCLES 12
#define STOP_CYCLES 12

// -----------------------------------------------------------------------------
static int process_halt_state(gbx_context_t *ctx)
{
    if (ctx->int_en & ctx->int_flags) {
        // leave HALT mode if any interrupt is fired
        ctx->exec_flags &= ~EXEC_HALT;
        return 0;
    }

    // remain in HALT mode. drive the rest of the system forward
    ctx->cycle_delta = STOP_CYCLES;
    perform_cyclic_tasks(ctx);
    return 1;
}

// -----------------------------------------------------------------------------
static int process_stop_state(gbx_context_t *ctx)
{
    if (ctx->key1 & KEY1_PREP) {
        // perform speed change, toggle speed mode and clear flag
        ctx->exec_flags &= ~EXEC_STOP;
        ctx->key1 = (ctx->key1 ^ KEY1_SPEED) & ~KEY1_PREP;
        ext_speed_change(ctx->userdata, ctx->key1 & KEY1_SPEED);
        return 0;
    }

    if (ctx->int_flags) {
        // leave STOP mode if any interrupt is fired
        ctx->exec_flags &= ~EXEC_STOP;
        return 0;
    }

    ctx->cycle_delta = STOP_CYCLES;
    perform_cyclic_tasks(ctx);
    return 1;
}

#define OPCODE          ctx->opcode1
#define OPCODE_CB       ctx->opcode2
#define IMM8            gbx_next_byte(ctx)
#define IMM16           gbx_next_word(ctx)
#define OP_0(n)         op_##n(ctx); break
#define OP_1(n, a)      op_##n(ctx, a); break
#define OP_2(n, a, b)   op_##n(ctx, a, b); break

// -----------------------------------------------------------------------------
long gbx_execute_cycles(gbx_context_t *ctx, long cycles_left)
{
    while (cycles_left > 0) {
        if (ctx->exec_flags) {
            // allow requests to terminate execution
            if (ctx->exec_flags & EXEC_BREAK)
                break;

            // if the cpu is halted, wait for an interrupt to be raised
            if ((ctx->exec_flags & EXEC_HALT) && process_halt_state(ctx)) {
                cycles_left -= HALT_CYCLES;
                continue;
            }

            // if the cpu is stopped, wait for a joypad interrupt
            if ((ctx->exec_flags & EXEC_STOP) && process_stop_state(ctx)) {
                cycles_left -= STOP_CYCLES;
                continue;
            }

            if (ctx->exec_flags & EXEC_TRACE)
                gbx_trace_instruction(ctx);
        }

        // fetch and store the next opcode
        ctx->next_pc = rPC;
        ctx->opcode1 = gbx_next_byte(ctx);
        ctx->cycle_delta = 0;

        // check prefix and execute the current instruction
        if (ctx->opcode1 == 0xCB) {
            ctx->opcode2 = gbx_next_byte(ctx);
#           include "decode_cb.inc"
            ctx->cycle_delta += gbx_instruction_cycles_cb[ctx->opcode2];
        }
        else {
#           include "decode.inc"
            ctx->cycle_delta += gbx_instruction_cycles[ctx->opcode1];
        }

        rPC = (ctx->next_pc & 0xFFFF);

        ctx->cycle_delta <<= 2;
        perform_cyclic_tasks(ctx);
        cycles_left -= ctx->cycle_delta;
    }

    return 0;
}

