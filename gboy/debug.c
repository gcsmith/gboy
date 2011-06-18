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
#include <string.h>
#include "gbx.h"
#include "memory.h"

static const char r8[] = "FACBEDLH";
static const char *r16[] = { "AF", "BC", "DE", "HL", "SP", "PC" };

#define asm_daa                 snprintf(o, s, "daa")
#define asm_cpl                 snprintf(o, s, "cpl")
#define asm_ccf                 snprintf(o, s, "ccf")
#define asm_scf                 snprintf(o, s, "scf")
#define asm_nop                 snprintf(o, s, "nop")
#define asm_halt                snprintf(o, s, "halt")
#define asm_stop                snprintf(o, s, "stop")
#define asm_di                  snprintf(o, s, "di")
#define asm_ei                  snprintf(o, s, "ei")
#define asm_rlca                snprintf(o, s, "rlca")
#define asm_rlci                snprintf(o, s, "rlc (HL)")
#define asm_rlc(rd)             snprintf(o, s, "rlc %c", r8[rd])
#define asm_rla                 snprintf(o, s, "rla")
#define asm_rli                 snprintf(o, s, "rl (HL)")
#define asm_rl(rd)              snprintf(o, s, "rl %c", r8[rd])
#define asm_rrca                snprintf(o, s, "rrca")
#define asm_rrci                snprintf(o, s, "rrc (HL)")
#define asm_rrc(rd)             snprintf(o, s, "rrc %c", r8[rd])
#define asm_rra                 snprintf(o, s, "rra")
#define asm_rri                 snprintf(o, s, "rr (HL)")
#define asm_rr(rd)              snprintf(o, s, "rr %c", r8[rd])
#define asm_ldac                snprintf(o, s, "ld ($FF00 + C), A")
#define asm_slai                snprintf(o, s, "sla (HL)")
#define asm_sla(rd)             snprintf(o, s, "sla %c", r8[rd])
#define asm_srai                snprintf(o, s, "sra (HL)")
#define asm_sra(rd)             snprintf(o, s, "sra %c", r8[rd])
#define asm_swapi               snprintf(o, s, "swap (HL)")
#define asm_swap(rd)            snprintf(o, s, "swap %c", r8[rd])
#define asm_srli                snprintf(o, s, "srl (HL)")
#define asm_srl(rd)             snprintf(o, s, "srl %c", r8[rd])
#define asm_bit(b, rd)          snprintf(o, s, "bit %d, %c", b, r8[rd])
#define asm_biti(b)             snprintf(o, s, "biti %d, (HL)n", b)
#define asm_res(b, rd)          snprintf(o, s, "res %d, %c", b, r8[rd])
#define asm_resi(b)             snprintf(o, s, "resi %d, (HL)n", b)
#define asm_set(b, rd)          snprintf(o, s, "set %d, %c", b, r8[rd])
#define asm_seti(b)             snprintf(o, s, "seti %d, (HL)n", b)
#define asm_ldca                snprintf(o, s, "ld A, ($FF00 + C)")
#define asm_ldd_ld              snprintf(o, s, "ldd A, (HL)")
#define asm_ldd_st              snprintf(o, s, "ldd (HL), A")
#define asm_ldi_ld              snprintf(o, s, "ldi A, (HL)")
#define asm_ldi_st              snprintf(o, s, "ldi (HL), A")
#define asm_ldsp_ld             snprintf(o, s, "ld SP, HL")
#define asm_ldsp_st(i)          snprintf(o, s, "ld (%04X), SP", i)
#define asm_ldhl(i)             snprintf(o, s, "ldhl SP, %02X", i)
#define asm_ldh_ld(i)           snprintf(o, s, "ldh A, ($FF00 + %02X)", i)
#define asm_ldh_st(i)           snprintf(o, s, "ldh ($FF00 + %02X), A", i)
#define asm_ld_r8r8(rs, rd)     snprintf(o, s, "ld %c, %c", r8[rd], r8[rs])
#define asm_ld_r8m8(rs, i)      snprintf(o, s, "ld (%04X), %c", i, r8[rs])
#define asm_ld_r8ir16(rs, rd)   snprintf(o, s, "ld (%s), %c", r16[rd], r8[rs])
#define asm_ld_n8r8(i, rd)      snprintf(o, s, "ld %c, %02X", r8[rd], i)
#define asm_ld_n8ir16(i, rd)    snprintf(o, s, "ld (%s), %02X", r16[rd], i)
#define asm_ld_m8r8(i, rd)      snprintf(o, s, "ld %c, (%04X)", r8[rd], i)
#define asm_ld_n16r16(i, rd)    snprintf(o, s, "ld %s, %04X", r16[rd], i)
#define asm_ld_ir16r8(rs, rd)   snprintf(o, s, "ld %c, (%s)", r8[rd], r16[rs])
#define asm_push(rs)            snprintf(o, s, "push %s", r16[rs])
#define asm_pop(rd)             snprintf(o, s, "pop %s", r16[rd])
#define asm_popaf               snprintf(o, s, "pop AF")
#define asm_cp(rs)              snprintf(o, s, "cp A, %c", r8[rs])
#define asm_cpn(i)              snprintf(o, s, "cp A, %02X", i)
#define asm_cpi(rs)             snprintf(o, s, "cp A, (%s)", r16[rs])
#define asm_and(rs)             snprintf(o, s, "and A, %c", r8[rs])
#define asm_andn(i)             snprintf(o, s, "and A, %02X", i)
#define asm_andi                snprintf(o, s, "and A, (HL)")
#define asm_or(rs)              snprintf(o, s, "or A, %c", r8[rs])
#define asm_orn(i)              snprintf(o, s, "or A, %02X", i)
#define asm_ori                 snprintf(o, s, "or A, (HL)")
#define asm_xor(rs)             snprintf(o, s, "xor A, %c", r8[rs])
#define asm_xorn(i)             snprintf(o, s, "xor A, %02X", i)
#define asm_xori                snprintf(o, s, "xor A, (HL)")
#define asm_addsp(i)            snprintf(o, s, "add SP, %02X", i)
#define asm_add(rs)             snprintf(o, s, "add A, %c", r8[rs])
#define asm_add_r16(rs)         snprintf(o, s, "add HL, %s", r16[rs])
#define asm_addn(i)             snprintf(o, s, "add A, %02X", i)
#define asm_addi                snprintf(o, s, "add A, (HL)")
#define asm_sub(rs)             snprintf(o, s, "sub A, %c", r8[rs])
#define asm_subn(i)             snprintf(o, s, "sub A, %02X", i)
#define asm_subi                snprintf(o, s, "sub A, (HL)")
#define asm_adc(rs)             snprintf(o, s, "adc A, %c", r8[rs])
#define asm_adcn(i)             snprintf(o, s, "adc A, %02X", i)
#define asm_adci                snprintf(o, s, "adc A, (HL)")
#define asm_sbc(rs)             snprintf(o, s, "sbc A, %c", r8[rs])
#define asm_sbcn(i)             snprintf(o, s, "sbc A, %02X", i)
#define asm_sbci(rs)            snprintf(o, s, "sbc A, (%s)", r16[rs])
#define asm_incb(rd)            snprintf(o, s, "inc %c", r8[rd])
#define asm_incw(rd)            snprintf(o, s, "inc %s", r16[rd])
#define asm_inci                snprintf(o, s, "inc (HL)")
#define asm_decb(rd)            snprintf(o, s, "dec %c", r8[rd])
#define asm_decw(rd)            snprintf(o, s, "dec %s", r16[rd])
#define asm_deci                snprintf(o, s, "dec (HL)")
#define asm_jp(i)               snprintf(o, s, "jp %04X", i)
#define asm_jpi                 snprintf(o, s, "jp (HL)")
#define asm_jpnz(i)             snprintf(o, s, "jp nz, %04X", i)
#define asm_jpz(i)              snprintf(o, s, "jp z, %04X", i)
#define asm_jpnc(i)             snprintf(o, s, "jp nc, %04X", i)
#define asm_jpc(i)              snprintf(o, s, "jp c, %04X", i)
#define asm_jr(i)               snprintf(o, s, "jr %02X", i)
#define asm_jrnz(i)             snprintf(o, s, "jr nz, %02X", i)
#define asm_jrz(i)              snprintf(o, s, "jr z, %02X", i)
#define asm_jrnc(i)             snprintf(o, s, "jr nc, %02X", i)
#define asm_jrc(i)              snprintf(o, s, "jr c, %02X", i)
#define asm_call(i)             snprintf(o, s, "call %04X", i)
#define asm_callnz(i)           snprintf(o, s, "call nz, %04X", i)
#define asm_callz(i)            snprintf(o, s, "call z, %04X", i)
#define asm_callnc(i)           snprintf(o, s, "call nc, %04X", i)
#define asm_callc(i)            snprintf(o, s, "call c, %04X", i)
#define asm_ret                 snprintf(o, s, "ret")
#define asm_retnz               snprintf(o, s, "ret nz")
#define asm_retz                snprintf(o, s, "ret z")
#define asm_retnc               snprintf(o, s, "ret nc")
#define asm_retc                snprintf(o, s, "ret c")
#define asm_reti                snprintf(o, s, "reti")
#define asm_rst(i)              snprintf(o, s, "rst %02X", i)
#define asm_reserved            snprintf(o, s, "invalid opcode")

#define OPCODE          ctx->opcode1
#define OPCODE_CB       ctx->opcode2
#define IMM8            gbx_next_byte(ctx)
#define IMM16           gbx_next_word(ctx)
#define OP_0(n)         asm_##n; break
#define OP_1(n, a)      asm_##n(a); break
#define OP_2(n, a, b)   asm_##n(a, b); break

// -----------------------------------------------------------------------------
int gbx_disassemble_op(gbx_context_t *ctx, char *o, int s)
{
    ctx->next_pc = ctx->reg.pc;
    ctx->bytes_read = 0;

    ctx->opcode1 = gbx_next_byte(ctx);
    if (ctx->opcode1 == 0xCB) {
        ctx->opcode2 = gbx_next_byte(ctx);
#include "decode_cb.inc"
    }
    else {
#include "decode.inc"
    }

    return ctx->bytes_read;
}

// -----------------------------------------------------------------------------
void gbx_trace_instruction(gbx_context_t *ctx)
{
    char buffer[256];
    int i;
    
    ctx->bytes_read = 0;
    gbx_disassemble_op(ctx, buffer, 256);

    log_dbg("%04X  ", ctx->reg.pc);
    for (i = 0; i < ctx->bytes_read; i++)
        log_dbg("%02X ", gbx_read_byte(ctx, ctx->reg.pc + i));
    for (i = ctx->bytes_read; i < 3; i++) log_dbg("   ");

    log_dbg(" %-20s  ", buffer);
    log_dbg("A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
            "SP:%04X Cy:%08X\n",
            ctx->reg.a, ctx->reg.f, ctx->reg.b, ctx->reg.c, ctx->reg.d,
            ctx->reg.e, ctx->reg.h, ctx->reg.l, ctx->reg.sp, ctx->cycles);
}

