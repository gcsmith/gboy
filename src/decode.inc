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

switch (OPCODE) {                           // Mnemonic     By  Cy  Z N H C
case 0x00: OP_0(nop);                       // NOP          1   4   - - - - 
case 0x01: OP_2(ld_n16r16, IMM16, REG_BC);  // LD BC,d16    3  12   - - - - 
case 0x02: OP_2(ld_r8ir16, REG_A, REG_BC);  // LD (BC),A    1   8   - - - - 
case 0x03: OP_1(incw, REG_BC);              // INC BC       1   8   - - - - 
case 0x04: OP_1(incb, REG_B);               // INC B        1   4   Z 0 H - 
case 0x05: OP_1(decb, REG_B);               // DEC B        1   4   Z 1 H - 
case 0x06: OP_2(ld_n8r8, IMM8, REG_B);      // LD B,d8      2   8   - - - - 
case 0x07: OP_0(rlca);                      // RLCA         1   4   0 0 0 C 
case 0x08: OP_1(ldsp_st, IMM16);            // LD (a16),SP  3  20   - - - - 
case 0x09: OP_1(add_r16, REG_BC);           // ADD HL,BC    1   8   - 0 H C 
case 0x0A: OP_2(ld_ir16r8, REG_BC, REG_A);  // LD A,(BC)    1   8   - - - - 
case 0x0B: OP_1(decw, REG_BC);              // DEC BC       1   8   - - - - 
case 0x0C: OP_1(incb, REG_C);               // INC C        1   4   Z 0 H - 
case 0x0D: OP_1(decb, REG_C);               // DEC C        1   4   Z 1 H - 
case 0x0E: OP_2(ld_n8r8, IMM8, REG_C);      // LD C,d8      2   8   - - - - 
case 0x0F: OP_0(rrca);                      // RRCA         1   4   0 0 0 C
case 0x10: OP_0(stop);                      // STOP 0       2   4   - - - -    
case 0x11: OP_2(ld_n16r16, IMM16, REG_DE);  // LD DE,d16    3  12   - - - -    
case 0x12: OP_2(ld_r8ir16, REG_A, REG_DE);  // LD (DE),A    1   8   - - - -    
case 0x13: OP_1(incw, REG_DE);              // INC DE       1   8   - - - -    
case 0x14: OP_1(incb, REG_D);               // INC D        1   4   Z 0 H -    
case 0x15: OP_1(decb, REG_D);               // DEC D        1   4   Z 1 H -    
case 0x16: OP_2(ld_n8r8, IMM8, REG_D);      // LD D,d8      2   8   - - - -
case 0x17: OP_0(rla);                       // RLA          1   4   0 0 0 C
case 0x18: OP_1(jr, IMM8);                  // JR r8        2  12   - - - -
case 0x19: OP_1(add_r16, REG_DE);           // ADD HL,DE    1   8   - 0 H C
case 0x1A: OP_2(ld_ir16r8, REG_DE, REG_A);  // LD A,(DE)    1   8   - - - - 
case 0x1B: OP_1(decw, REG_DE);              // DEC DE       1   8   - - - -
case 0x1C: OP_1(incb, REG_E);               // INC E        1   4   Z 0 H -
case 0x1D: OP_1(decb, REG_E);               // DEC E        1   4   Z 1 H -
case 0x1E: OP_2(ld_n8r8, IMM8, REG_E);      // LD E,d8      2   8   - - - -
case 0x1F: OP_0(rra);                       // RRA          1   4   0 0 0 C
case 0x20: OP_1(jrnz, IMM8);                // JR NZ,r8     2  12/8 - - - -    
case 0x21: OP_2(ld_n16r16, IMM16, REG_HL);  // LD HL,d16    3  12   - - - -    
case 0x22: OP_0(ldi_st);                    // LD (HL+),A   1   8   - - - -    
case 0x23: OP_1(incw, REG_HL);              // INC HL       1   8   - - - -    
case 0x24: OP_1(incb, REG_H);               // INC H        1   4   Z 0 H -    
case 0x25: OP_1(decb, REG_H);               // DEC H        1   4   Z 1 H -    
case 0x26: OP_2(ld_n8r8, IMM8, REG_H);      // LD H,d8      2   8   - - - -    
case 0x27: OP_0(daa);                       // DAA          1   4   Z - 0 C    
case 0x28: OP_1(jrz, IMM8);                 // JR Z,r8      2  12/8 - - - -    
case 0x29: OP_1(add_r16, REG_HL);           // ADD HL,HL    1   8   - 0 H C    
case 0x2A: OP_0(ldi_ld);                    // LD A,(HL+)   1   8   - - - -    
case 0x2B: OP_1(decw, REG_HL);              // DEC HL       1   8   - - - -    
case 0x2C: OP_1(incb, REG_L);               // INC L        1   4   Z 0 H -    
case 0x2D: OP_1(decb, REG_L);               // DEC L        1   4   Z 1 H -    
case 0x2E: OP_2(ld_n8r8, IMM8, REG_L);      // LD L,d8      2   8   - - - -    
case 0x2F: OP_0(cpl);                       // CPL          1   4   - 1 1 -
case 0x30: OP_1(jrnc, IMM8);                // JR NC,r8     2  12/8 - - - -    
case 0x31: OP_2(ld_n16r16, IMM16, REG_SP);  // LD SP,d16    3  12   - - - -    
case 0x32: OP_0(ldd_st);                    // LD (HL-),A   1   8   - - - -    
case 0x33: OP_1(incw, REG_SP);              // INC SP       1   8   - - - -    
case 0x34: OP_0(inci);                      // INC (HL)     1  12   Z 0 H -    
case 0x35: OP_0(deci);                      // DEC (HL)     1  12   Z 1 H -    
case 0x36: OP_2(ld_n8ir16, IMM8, REG_HL);   // LD (HL),d8   2  12   - - - -    
case 0x37: OP_0(scf);                       // SCF          1   4   - 0 0 1    
case 0x38: OP_1(jrc, IMM8);                 // JR C,r8      2  12/8 - - - -    
case 0x39: OP_1(add_r16, REG_SP);           // ADD HL,SP    1   8   - 0 H C    
case 0x3A: OP_0(ldd_ld);                    // LD A,(HL-)   1   8   - - - -    
case 0x3B: OP_1(decw, REG_SP);              // DEC SP       1   8   - - - -    
case 0x3C: OP_1(incb, REG_A);               // INC A        1   4   Z 0 H -    
case 0x3D: OP_1(decb, REG_A);               // DEC A        1   4   Z 1 H -    
case 0x3E: OP_2(ld_n8r8, IMM8, REG_A);      // LD A,d8      2   8   - - - -    
case 0x3F: OP_0(ccf);                       // CCF          1   4   - 0 0 C
case 0x40: OP_2(ld_r8r8, REG_B, REG_B);     // LD B,B       1   4   - - - -    
case 0x41: OP_2(ld_r8r8, REG_C, REG_B);     // LD B,C       1   4   - - - -    
case 0x42: OP_2(ld_r8r8, REG_D, REG_B);     // LD B,D       1   4   - - - -    
case 0x43: OP_2(ld_r8r8, REG_E, REG_B);     // LD B,E       1   4   - - - -    
case 0x44: OP_2(ld_r8r8, REG_H, REG_B);     // LD B,H       1   4   - - - -    
case 0x45: OP_2(ld_r8r8, REG_L, REG_B);     // LD B,L       1   4   - - - -    
case 0x46: OP_2(ld_ir16r8, REG_HL, REG_B);  // LD B,(HL)    1   8   - - - -    
case 0x47: OP_2(ld_r8r8, REG_A, REG_B);     // LD B,A       1   4   - - - -    
case 0x48: OP_2(ld_r8r8, REG_B, REG_C);     // LD C,B       1   4   - - - -    
case 0x49: OP_2(ld_r8r8, REG_C, REG_C);     // LD C,C       1   4   - - - -    
case 0x4A: OP_2(ld_r8r8, REG_D, REG_C);     // LD C,D       1   4   - - - -    
case 0x4B: OP_2(ld_r8r8, REG_E, REG_C);     // LD C,E       1   4   - - - -    
case 0x4C: OP_2(ld_r8r8, REG_H, REG_C);     // LD C,H       1   4   - - - -    
case 0x4D: OP_2(ld_r8r8, REG_L, REG_C);     // LD C,L       1   4   - - - -    
case 0x4E: OP_2(ld_ir16r8, REG_HL, REG_C);  // LD C,(HL)    1   8   - - - -    
case 0x4F: OP_2(ld_r8r8, REG_A, REG_C);     // LD C,A       1   4   - - - -
case 0x50: OP_2(ld_r8r8, REG_B, REG_D);     // LD D,B       1   4   - - - -    
case 0x51: OP_2(ld_r8r8, REG_C, REG_D);     // LD D,C       1   4   - - - -    
case 0x52: OP_2(ld_r8r8, REG_D, REG_D);     // LD D,D       1   4   - - - -    
case 0x53: OP_2(ld_r8r8, REG_E, REG_D);     // LD D,E       1   4   - - - -    
case 0x54: OP_2(ld_r8r8, REG_H, REG_D);     // LD D,H       1   4   - - - -    
case 0x55: OP_2(ld_r8r8, REG_L, REG_D);     // LD D,L       1   4   - - - -    
case 0x56: OP_2(ld_ir16r8, REG_HL, REG_D);  // LD D,(HL)    1   8   - - - -    
case 0x57: OP_2(ld_r8r8, REG_A, REG_D);     // LD D,A       1   4   - - - -    
case 0x58: OP_2(ld_r8r8, REG_B, REG_E);     // LD E,B       1   4   - - - -    
case 0x59: OP_2(ld_r8r8, REG_C, REG_E);     // LD E,C       1   4   - - - -    
case 0x5A: OP_2(ld_r8r8, REG_D, REG_E);     // LD E,D       1   4   - - - -    
case 0x5B: OP_2(ld_r8r8, REG_E, REG_E);     // LD E,E       1   4   - - - -    
case 0x5C: OP_2(ld_r8r8, REG_H, REG_E);     // LD E,H       1   4   - - - -    
case 0x5D: OP_2(ld_r8r8, REG_L, REG_E);     // LD E,L       1   4   - - - -    
case 0x5E: OP_2(ld_ir16r8, REG_HL, REG_E);  // LD E,(HL)    1   8   - - - -    
case 0x5F: OP_2(ld_r8r8, REG_A, REG_E);     // LD E,A       1   4   - - - -
case 0x60: OP_2(ld_r8r8, REG_B, REG_H);     // LD H,B       1   4   - - - -    
case 0x61: OP_2(ld_r8r8, REG_C, REG_H);     // LD H,C       1   4   - - - -    
case 0x62: OP_2(ld_r8r8, REG_D, REG_H);     // LD H,D       1   4   - - - -    
case 0x63: OP_2(ld_r8r8, REG_E, REG_H);     // LD H,E       1   4   - - - -    
case 0x64: OP_2(ld_r8r8, REG_H, REG_H);     // LD H,H       1   4   - - - -    
case 0x65: OP_2(ld_r8r8, REG_L, REG_H);     // LD H,L       1   4   - - - -    
case 0x66: OP_2(ld_ir16r8, REG_HL, REG_H);  // LD H,(HL)    1   8   - - - -    
case 0x67: OP_2(ld_r8r8, REG_A, REG_H);     // LD H,A       1   4   - - - -    
case 0x68: OP_2(ld_r8r8, REG_B, REG_L);     // LD L,B       1   4   - - - -    
case 0x69: OP_2(ld_r8r8, REG_C, REG_L);     // LD L,C       1   4   - - - -    
case 0x6A: OP_2(ld_r8r8, REG_D, REG_L);     // LD L,D       1   4   - - - -    
case 0x6B: OP_2(ld_r8r8, REG_E, REG_L);     // LD L,E       1   4   - - - -    
case 0x6C: OP_2(ld_r8r8, REG_H, REG_L);     // LD L,H       1   4   - - - -    
case 0x6D: OP_2(ld_r8r8, REG_L, REG_L);     // LD L,L       1   4   - - - -    
case 0x6E: OP_2(ld_ir16r8, REG_HL, REG_L);  // LD L,(HL)    1   8   - - - -    
case 0x6F: OP_2(ld_r8r8, REG_A, REG_L);     // LD L,A       1   4   - - - -
case 0x70: OP_2(ld_r8ir16, REG_B, REG_HL);  // LD (HL),B    1   8   - - - -    
case 0x71: OP_2(ld_r8ir16, REG_C, REG_HL);  // LD (HL),C    1   8   - - - -    
case 0x72: OP_2(ld_r8ir16, REG_D, REG_HL);  // LD (HL),D    1   8   - - - -    
case 0x73: OP_2(ld_r8ir16, REG_E, REG_HL);  // LD (HL),E    1   8   - - - -    
case 0x74: OP_2(ld_r8ir16, REG_H, REG_HL);  // LD (HL),H    1   8   - - - -    
case 0x75: OP_2(ld_r8ir16, REG_L, REG_HL);  // LD (HL),L    1   8   - - - -    
case 0x76: OP_0(halt);                      // HALT         1   4   - - - -    
case 0x77: OP_2(ld_r8ir16, REG_A, REG_HL);  // LD (HL),A    1   8   - - - -    
case 0x78: OP_2(ld_r8r8, REG_B, REG_A);     // LD A,B       1   4   - - - -    
case 0x79: OP_2(ld_r8r8, REG_C, REG_A);     // LD A,C       1   4   - - - -    
case 0x7A: OP_2(ld_r8r8, REG_D, REG_A);     // LD A,D       1   4   - - - -    
case 0x7B: OP_2(ld_r8r8, REG_E, REG_A);     // LD A,E       1   4   - - - -    
case 0x7C: OP_2(ld_r8r8, REG_H, REG_A);     // LD A,H       1   4   - - - -    
case 0x7D: OP_2(ld_r8r8, REG_L, REG_A);     // LD A,L       1   4   - - - -    
case 0x7E: OP_2(ld_ir16r8, REG_HL, REG_A);  // LD A,(HL)    1   8   - - - -    
case 0x7F: OP_2(ld_r8r8, REG_A, REG_A);     // LD A,A       1   4   - - - -
case 0x80: OP_1(add, REG_B);                // ADD A,B      1   4   Z 0 H C    
case 0x81: OP_1(add, REG_C);                // ADD A,C      1   4   Z 0 H C    
case 0x82: OP_1(add, REG_D);                // ADD A,D      1   4   Z 0 H C    
case 0x83: OP_1(add, REG_E);                // ADD A,E      1   4   Z 0 H C    
case 0x84: OP_1(add, REG_H);                // ADD A,H      1   4   Z 0 H C    
case 0x85: OP_1(add, REG_L);                // ADD A,L      1   4   Z 0 H C    
case 0x86: OP_0(addi);                      // ADD A,(HL)   1   8   Z 0 H C    
case 0x87: OP_1(add, REG_A);                // ADD A,A      1   4   Z 0 H C    
case 0x88: OP_1(adc, REG_B);                // ADC A,B      1   4   Z 0 H C    
case 0x89: OP_1(adc, REG_C);                // ADC A,C      1   4   Z 0 H C    
case 0x8A: OP_1(adc, REG_D);                // ADC A,D      1   4   Z 0 H C    
case 0x8B: OP_1(adc, REG_E);                // ADC A,E      1   4   Z 0 H C    
case 0x8C: OP_1(adc, REG_H);                // ADC A,H      1   4   Z 0 H C    
case 0x8D: OP_1(adc, REG_L);                // ADC A,L      1   4   Z 0 H C    
case 0x8E: OP_0(adci);                      // ADC A,(HL)   1   8   Z 0 H C    
case 0x8F: OP_1(adc, REG_A);                // ADC A,A      1   4   Z 0 H C
case 0x90: OP_1(sub, REG_B);                // SUB B        1   4   Z 1 H C   
case 0x91: OP_1(sub, REG_C);                // SUB C        1   4   Z 1 H C   
case 0x92: OP_1(sub, REG_D);                // SUB D        1   4   Z 1 H C   
case 0x93: OP_1(sub, REG_E);                // SUB E        1   4   Z 1 H C   
case 0x94: OP_1(sub, REG_H);                // SUB H        1   4   Z 1 H C   
case 0x95: OP_1(sub, REG_L);                // SUB L        1   4   Z 1 H C   
case 0x96: OP_0(subi);                      // SUB (HL)     1   8   Z 1 H C   
case 0x97: OP_1(sub, REG_A);                // SUB A        1   4   Z 1 H C   
case 0x98: OP_1(sbc, REG_B);                // SBC A,B      1   4   Z 1 H C   
case 0x99: OP_1(sbc, REG_C);                // SBC A,C      1   4   Z 1 H C   
case 0x9A: OP_1(sbc, REG_D);                // SBC A,D      1   4   Z 1 H C   
case 0x9B: OP_1(sbc, REG_E);                // SBC A,E      1   4   Z 1 H C   
case 0x9C: OP_1(sbc, REG_H);                // SBC A,H      1   4   Z 1 H C   
case 0x9D: OP_1(sbc, REG_L);                // SBC A,L      1   4   Z 1 H C   
case 0x9E: OP_1(sbci, REG_HL);              // SBC A,(HL)   1   8   Z 1 H C   
case 0x9F: OP_1(sbc, REG_A);                // SBC A,A      1   4   Z 1 H C
case 0xA0: OP_1(and, REG_B);                // AND B        1   4   Z 0 1 0  
case 0xA1: OP_1(and, REG_C);                // AND C        1   4   Z 0 1 0  
case 0xA2: OP_1(and, REG_D);                // AND D        1   4   Z 0 1 0  
case 0xA3: OP_1(and, REG_E);                // AND E        1   4   Z 0 1 0  
case 0xA4: OP_1(and, REG_H);                // AND H        1   4   Z 0 1 0  
case 0xA5: OP_1(and, REG_L);                // AND L        1   4   Z 0 1 0  
case 0xA6: OP_0(andi);                      // AND (HL)     1   8   Z 0 1 0  
case 0xA7: OP_1(and, REG_A);                // AND A        1   4   Z 0 1 0  
case 0xA8: OP_1(xor, REG_B);                // XOR B        1   4   Z 0 0 0  
case 0xA9: OP_1(xor, REG_C);                // XOR C        1   4   Z 0 0 0  
case 0xAA: OP_1(xor, REG_D);                // XOR D        1   4   Z 0 0 0  
case 0xAB: OP_1(xor, REG_E);                // XOR E        1   4   Z 0 0 0  
case 0xAC: OP_1(xor, REG_H);                // XOR H        1   4   Z 0 0 0  
case 0xAD: OP_1(xor, REG_L);                // XOR L        1   4   Z 0 0 0  
case 0xAE: OP_0(xori);                      // XOR (HL)     1   8   Z 0 0 0  
case 0xAF: OP_1(xor, REG_A);                // XOR A        1   4   Z 0 0 0
case 0xB0: OP_1(or, REG_B);                 // OR B         1   4   Z 0 0 0  
case 0xB1: OP_1(or, REG_C);                 // OR C         1   4   Z 0 0 0  
case 0xB2: OP_1(or, REG_D);                 // OR D         1   4   Z 0 0 0  
case 0xB3: OP_1(or, REG_E);                 // OR E         1   4   Z 0 0 0  
case 0xB4: OP_1(or, REG_H);                 // OR H         1   4   Z 0 0 0  
case 0xB5: OP_1(or, REG_L);                 // OR L         1   4   Z 0 0 0  
case 0xB6: OP_0(ori);                       // OR (HL)      1   8   Z 0 0 0  
case 0xB7: OP_1(or, REG_A);                 // OR A         1   4   Z 0 0 0  
case 0xB8: OP_1(cp, REG_B);                 // CP B         1   4   Z 1 H C  
case 0xB9: OP_1(cp, REG_C);                 // CP C         1   4   Z 1 H C  
case 0xBA: OP_1(cp, REG_D);                 // CP D         1   4   Z 1 H C  
case 0xBB: OP_1(cp, REG_E);                 // CP E         1   4   Z 1 H C  
case 0xBC: OP_1(cp, REG_H);                 // CP H         1   4   Z 1 H C  
case 0xBD: OP_1(cp, REG_L);                 // CP L         1   4   Z 1 H C  
case 0xBE: OP_1(cpi, REG_HL);               // CP (HL)      1   8   Z 1 H C  
case 0xBF: OP_1(cp, REG_A);                 // CP A         1   4   Z 1 H C
case 0xC0: OP_0(retnz);                     // RET NZ       1 20/8  - - - -  
case 0xC1: OP_1(pop, REG_BC);               // POP BC       1  12   - - - -  
case 0xC2: OP_1(jpnz, IMM16);               // JP NZ, a16   3 16/12 - - - -  
case 0xC3: OP_1(jp, IMM16);                 // JP a16       3  16   - - - -  
case 0xC4: OP_1(callnz, IMM16);             // CALL NZ, a16 3 24/12 - - - -  
case 0xC5: OP_1(push, REG_BC);              // PUSH BC      1  16   - - - -  
case 0xC6: OP_1(addn, IMM8);                // ADD A, d8    2   8   Z 0 H C  
case 0xC7: OP_1(rst, 0x00);                 // RST 00H      1  16   - - - -  
case 0xC8: OP_0(retz);                      // RET Z        1 20/8  - - - -  
case 0xC9: OP_0(ret);                       // RET          1  16   - - - -  
case 0xCA: OP_1(jpz, IMM16);                // JP Z, a16    3 16/12 - - - -  
case 0xCB: OP_0(reserved);
case 0xCC: OP_1(callz, IMM16);              // CALL Z, a16  3 24/12 - - - -  
case 0xCD: OP_1(call, IMM16);               // CALL a16     3  24   - - - -  
case 0xCE: OP_1(adcn, IMM8);                // ADC A, d8    2   8   Z 0 H C  
case 0xCF: OP_1(rst, 0x08);                 // RST 08H      1  16   - - - -
case 0xD0: OP_0(retnc);                     // RET NC       1 20/8  - - - - 
case 0xD1: OP_1(pop, REG_DE);               // POP DE       1  12   - - - - 
case 0xD2: OP_1(jpnc, IMM16);               // JP NC,a16    3 16/12 - - - -     
case 0xD3: OP_0(reserved);
case 0xD4: OP_1(callnc, IMM16);             // CALL NC,a16  3 24/12 - - - - 
case 0xD5: OP_1(push, REG_DE);              // PUSH DE      1  16   - - - - 
case 0xD6: OP_1(subn, IMM8);                // SUB d8       2  8    Z 1 H C 
case 0xD7: OP_1(rst, 0x10);                 // RST 10H      1  16   - - - - 
case 0xD8: OP_0(retc);                      // RET C        1 20/8  - - - - 
case 0xD9: OP_0(reti);                      // RETI         1  16   - - - - 
case 0xDA: OP_1(jpc, IMM16);                // JP C,a16     3 16/12 - - - -     
case 0xDB: OP_0(reserved);
case 0xDC: OP_1(callc, IMM16);              // CALL C,a16   3 24/12 - - - -     
case 0xDD: OP_0(reserved);
case 0xDE: OP_1(sbcn, IMM8);                // SBC A,d8     2   8   Z 1 H C 
case 0xDF: OP_1(rst, 0x18);                 // RST 18H      1  16   - - - -
case 0xE0: OP_1(ldh_st, IMM8);              // LDH (a8),A   2  12   - - - - 
case 0xE1: OP_1(pop, REG_HL);               // POP HL       1  12   - - - - 
case 0xE2: OP_0(ldac);                      // LD (C),A     2   8   - - - -         
case 0xE3: OP_0(reserved);
case 0xE4: OP_0(reserved);
case 0xE5: OP_1(push, REG_HL);              // PUSH HL      1  16   - - - - 
case 0xE6: OP_1(andn, IMM8);                // AND d8       2   8   Z 0 1 0 
case 0xE7: OP_1(rst, 0x20);                 // RST 20H      1  16   - - - - 
case 0xE8: OP_1(addsp, IMM8);               // ADD SP,r8    2  16   0 0 H C 
case 0xE9: OP_0(jpi);                       // JP (HL)      1   4   - - - - 
case 0xEA: OP_2(ld_r8m8, REG_A, IMM16);     // LD (a16),A   3  16   - - - -             
case 0xEB: OP_0(reserved);
case 0xEC: OP_0(reserved);
case 0xED: OP_0(reserved);
case 0xEE: OP_1(xorn, IMM8);                // XOR d8       2   8   Z 0 0 0 
case 0xEF: OP_1(rst, 0x28);                 // RST 28H      1  16   - - - -
case 0xF0: OP_1(ldh_ld, IMM8);              // LDH A,(a8)   2  12   - - - -    
case 0xF1: OP_0(popaf);                     // POP AF       1  12   Z N H C    
case 0xF2: OP_0(ldca);                      // LD A,(C)     2   8   - - - -    
case 0xF3: OP_0(di);                        // DI           1   4   - - - -        
case 0xF4: OP_0(reserved);
case 0xF5: OP_1(push, REG_AF);              // PUSH AF      1  16   - - - -    
case 0xF6: OP_1(orn, IMM8);                 // OR d8        2   8   Z 0 0 0    
case 0xF7: OP_1(rst, 0x30);                 // RST 30H      1  16   - - - -    
case 0xF8: OP_1(ldhl, IMM8);                // LD HL,SP+r8  2  12   0 0 H C    
case 0xF9: OP_0(ldsp_ld);                   // LD SP,HL     1   8   - - - -    
case 0xFA: OP_2(ld_m8r8, IMM16, REG_A);     // LD A,(a16)   3  16   - - - -    
case 0xFB: OP_0(ei);                        // EI           1   4   - - - -            
case 0xFC: OP_0(reserved);
case 0xFD: OP_0(reserved);
case 0xFE: OP_1(cpn, IMM8);                 // CP d8        2   8   Z 1 H C    
case 0xFF: OP_1(rst, 0x38);                 // RST 38H      1  16   - - - -
}

