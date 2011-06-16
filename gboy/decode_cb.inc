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

switch (OPCODE_CB) {
case 0x00: OP_1(rlc, REG_B);                // RLC B        2   8   Z 0 0 C    
case 0x01: OP_1(rlc, REG_C);                // RLC C        2   8   Z 0 0 C    
case 0x02: OP_1(rlc, REG_D);                // RLC D        2   8   Z 0 0 C    
case 0x03: OP_1(rlc, REG_E);                // RLC E        2   8   Z 0 0 C    
case 0x04: OP_1(rlc, REG_H);                // RLC H        2   8   Z 0 0 C    
case 0x05: OP_1(rlc, REG_L);                // RLC L        2   8   Z 0 0 C    
case 0x06: OP_0(rlci);                      // RLC (HL)     2  16   Z 0 0 C    
case 0x07: OP_1(rlc, REG_A);                // RLC A        2   8   Z 0 0 C    
case 0x08: OP_1(rrc, REG_B);                // RRC B        2   8   Z 0 0 C    
case 0x09: OP_1(rrc, REG_C);                // RRC C        2   8   Z 0 0 C    
case 0x0A: OP_1(rrc, REG_D);                // RRC D        2   8   Z 0 0 C    
case 0x0B: OP_1(rrc, REG_E);                // RRC E        2   8   Z 0 0 C    
case 0x0C: OP_1(rrc, REG_H);                // RRC H        2   8   Z 0 0 C    
case 0x0D: OP_1(rrc, REG_L);                // RRC L        2   8   Z 0 0 C    
case 0x0E: OP_0(rrci);                      // RRC (HL)     2  16   Z 0 0 C    
case 0x0F: OP_1(rrc, REG_A);                // RRC A        2   8   Z 0 0 C
case 0x10: OP_1(rl, REG_B);                 // RL B         2   8   Z 0 0 C   
case 0x11: OP_1(rl, REG_C);                 // RL C         2   8   Z 0 0 C   
case 0x12: OP_1(rl, REG_D);                 // RL D         2   8   Z 0 0 C   
case 0x13: OP_1(rl, REG_E);                 // RL E         2   8   Z 0 0 C   
case 0x14: OP_1(rl, REG_H);                 // RL H         2   8   Z 0 0 C   
case 0x15: OP_1(rl, REG_L);                 // RL L         2   8   Z 0 0 C   
case 0x16: OP_0(rli);                       // RL (HL)      2  16   Z 0 0 C   
case 0x17: OP_1(rl, REG_A);                 // RL A         2   8   Z 0 0 C   
case 0x18: OP_1(rr, REG_B);                 // RR B         2   8   Z 0 0 C   
case 0x19: OP_1(rr, REG_C);                 // RR C         2   8   Z 0 0 C   
case 0x1A: OP_1(rr, REG_D);                 // RR D         2   8   Z 0 0 C   
case 0x1B: OP_1(rr, REG_E);                 // RR E         2   8   Z 0 0 C   
case 0x1C: OP_1(rr, REG_H);                 // RR H         2   8   Z 0 0 C   
case 0x1D: OP_1(rr, REG_L);                 // RR L         2   8   Z 0 0 C   
case 0x1E: OP_0(rri);                       // RR (HL)      2  16   Z 0 0 C   
case 0x1F: OP_1(rr, REG_A);                 // RR A         2   8   Z 0 0 C
case 0x20: OP_1(sla, REG_B);                // SLA B        2   8   Z 0 0 C  
case 0x21: OP_1(sla, REG_C);                // SLA C        2   8   Z 0 0 C  
case 0x22: OP_1(sla, REG_D);                // SLA D        2   8   Z 0 0 C  
case 0x23: OP_1(sla, REG_E);                // SLA E        2   8   Z 0 0 C  
case 0x24: OP_1(sla, REG_H);                // SLA H        2   8   Z 0 0 C  
case 0x25: OP_1(sla, REG_L);                // SLA L        2   8   Z 0 0 C  
case 0x26: OP_0(slai);                      // SLA (HL)     2  16   Z 0 0 C  
case 0x27: OP_1(sla, REG_A);                // SLA A        2   8   Z 0 0 C  
case 0x28: OP_1(sra, REG_B);                // SRA B        2   8   Z 0 0 0  
case 0x29: OP_1(sra, REG_C);                // SRA C        2   8   Z 0 0 0  
case 0x2A: OP_1(sra, REG_D);                // SRA D        2   8   Z 0 0 0  
case 0x2B: OP_1(sra, REG_E);                // SRA E        2   8   Z 0 0 0  
case 0x2C: OP_1(sra, REG_H);                // SRA H        2   8   Z 0 0 0  
case 0x2D: OP_1(sra, REG_L);                // SRA L        2   8   Z 0 0 0  
case 0x2E: OP_0(srai);                      // SRA (HL)     2  16   Z 0 0 0  
case 0x2F: OP_1(sra, REG_A);                // SRA A        2   8   Z 0 0 0
case 0x30: OP_1(swap, REG_B);               // SWAP B       2   8   Z 0 0 0 
case 0x31: OP_1(swap, REG_C);               // SWAP C       2   8   Z 0 0 0 
case 0x32: OP_1(swap, REG_D);               // SWAP D       2   8   Z 0 0 0 
case 0x33: OP_1(swap, REG_E);               // SWAP E       2   8   Z 0 0 0 
case 0x34: OP_1(swap, REG_H);               // SWAP H       2   8   Z 0 0 0 
case 0x35: OP_1(swap, REG_L);               // SWAP L       2   8   Z 0 0 0 
case 0x36: OP_0(swapi);                     // SWAP (HL)    2  16   Z 0 0 0 
case 0x37: OP_1(swap, REG_A);               // SWAP A       2   8   Z 0 0 0 
case 0x38: OP_1(srl, REG_B);                // SRL B        2   8   Z 0 0 C 
case 0x39: OP_1(srl, REG_C);                // SRL C        2   8   Z 0 0 C 
case 0x3A: OP_1(srl, REG_D);                // SRL D        2   8   Z 0 0 C 
case 0x3B: OP_1(srl, REG_E);                // SRL E        2   8   Z 0 0 C 
case 0x3C: OP_1(srl, REG_H);                // SRL H        2   8   Z 0 0 C 
case 0x3D: OP_1(srl, REG_L);                // SRL L        2   8   Z 0 0 C 
case 0x3E: OP_0(srli);                      // SRL (HL)     2  16   Z 0 0 C 
case 0x3F: OP_1(srl, REG_A);                // SRL A        2   8   Z 0 0 C
case 0x40: OP_2(bit, 0, REG_B);             // BIT 0,B      2   8   Z 0 1 -    
case 0x41: OP_2(bit, 0, REG_C);             // BIT 0,C      2   8   Z 0 1 -    
case 0x42: OP_2(bit, 0, REG_D);             // BIT 0,D      2   8   Z 0 1 -    
case 0x43: OP_2(bit, 0, REG_E);             // BIT 0,E      2   8   Z 0 1 -    
case 0x44: OP_2(bit, 0, REG_H);             // BIT 0,H      2   8   Z 0 1 -    
case 0x45: OP_2(bit, 0, REG_L);             // BIT 0,L      2   8   Z 0 1 -    
case 0x46: OP_1(biti, 0);                   // BIT 0,(HL)   2  16   Z 0 1 -    
case 0x47: OP_2(bit, 0, REG_A);             // BIT 0,A      2   8   Z 0 1 -    
case 0x48: OP_2(bit, 1, REG_B);             // BIT 1,B      2   8   Z 0 1 -    
case 0x49: OP_2(bit, 1, REG_C);             // BIT 1,C      2   8   Z 0 1 -    
case 0x4A: OP_2(bit, 1, REG_D);             // BIT 1,D      2   8   Z 0 1 -    
case 0x4B: OP_2(bit, 1, REG_E);             // BIT 1,E      2   8   Z 0 1 -    
case 0x4C: OP_2(bit, 1, REG_H);             // BIT 1,H      2   8   Z 0 1 -    
case 0x4D: OP_2(bit, 1, REG_L);             // BIT 1,L      2   8   Z 0 1 -    
case 0x4E: OP_1(biti, 1);                   // BIT 1,(HL)   2  16   Z 0 1 -    
case 0x4F: OP_2(bit, 1, REG_A);             // BIT 1,A      2   8   Z 0 1 -
case 0x50: OP_2(bit, 2, REG_B);             // BIT 2,B      2   8   Z 0 1 -   
case 0x51: OP_2(bit, 2, REG_C);             // BIT 2,C      2   8   Z 0 1 -   
case 0x52: OP_2(bit, 2, REG_D);             // BIT 2,D      2   8   Z 0 1 -   
case 0x53: OP_2(bit, 2, REG_E);             // BIT 2,E      2   8   Z 0 1 -   
case 0x54: OP_2(bit, 2, REG_H);             // BIT 2,H      2   8   Z 0 1 -   
case 0x55: OP_2(bit, 2, REG_L);             // BIT 2,L      2   8   Z 0 1 -   
case 0x56: OP_1(biti, 2);                   // BIT 2,(HL)   2  16   Z 0 1 -   
case 0x57: OP_2(bit, 2, REG_A);             // BIT 2,A      2   8   Z 0 1 -   
case 0x58: OP_2(bit, 3, REG_B);             // BIT 3,B      2   8   Z 0 1 -   
case 0x59: OP_2(bit, 3, REG_C);             // BIT 3,C      2   8   Z 0 1 -   
case 0x5A: OP_2(bit, 3, REG_D);             // BIT 3,D      2   8   Z 0 1 -   
case 0x5B: OP_2(bit, 3, REG_E);             // BIT 3,E      2   8   Z 0 1 -   
case 0x5C: OP_2(bit, 3, REG_H);             // BIT 3,H      2   8   Z 0 1 -   
case 0x5D: OP_2(bit, 3, REG_L);             // BIT 3,L      2   8   Z 0 1 -   
case 0x5E: OP_1(biti, 3);                   // BIT 3,(HL)   2  16   Z 0 1 -   
case 0x5F: OP_2(bit, 3, REG_A);             // BIT 3,A      2   8   Z 0 1 -
case 0x60: OP_2(bit, 4, REG_B);             // BIT 4,B      2   8   Z 0 1 -  
case 0x61: OP_2(bit, 4, REG_C);             // BIT 4,C      2   8   Z 0 1 -  
case 0x62: OP_2(bit, 4, REG_D);             // BIT 4,D      2   8   Z 0 1 -  
case 0x63: OP_2(bit, 4, REG_E);             // BIT 4,E      2   8   Z 0 1 -  
case 0x64: OP_2(bit, 4, REG_H);             // BIT 4,H      2   8   Z 0 1 -  
case 0x65: OP_2(bit, 4, REG_L);             // BIT 4,L      2   8   Z 0 1 -  
case 0x66: OP_1(biti, 4);                   // BIT 4,(HL)   2  16   Z 0 1 -  
case 0x67: OP_2(bit, 4, REG_A);             // BIT 4,A      2   8   Z 0 1 -  
case 0x68: OP_2(bit, 5, REG_B);             // BIT 5,B      2   8   Z 0 1 -  
case 0x69: OP_2(bit, 5, REG_C);             // BIT 5,C      2   8   Z 0 1 -  
case 0x6A: OP_2(bit, 5, REG_D);             // BIT 5,D      2   8   Z 0 1 -  
case 0x6B: OP_2(bit, 5, REG_E);             // BIT 5,E      2   8   Z 0 1 -  
case 0x6C: OP_2(bit, 5, REG_H);             // BIT 5,H      2   8   Z 0 1 -  
case 0x6D: OP_2(bit, 5, REG_L);             // BIT 5,L      2   8   Z 0 1 -  
case 0x6E: OP_1(biti, 5);                   // BIT 5,(HL)   2  16   Z 0 1 -  
case 0x6F: OP_2(bit, 5, REG_A);             // BIT 5,A      2   8   Z 0 1 -
case 0x70: OP_2(bit, 6, REG_B);             // BIT 6,B      2   8   Z 0 1 - 
case 0x71: OP_2(bit, 6, REG_C);             // BIT 6,C      2   8   Z 0 1 - 
case 0x72: OP_2(bit, 6, REG_D);             // BIT 6,D      2   8   Z 0 1 - 
case 0x73: OP_2(bit, 6, REG_E);             // BIT 6,E      2   8   Z 0 1 - 
case 0x74: OP_2(bit, 6, REG_H);             // BIT 6,H      2   8   Z 0 1 - 
case 0x75: OP_2(bit, 6, REG_L);             // BIT 6,L      2   8   Z 0 1 - 
case 0x76: OP_1(biti, 6);                   // BIT 6,(HL)   2  16   Z 0 1 - 
case 0x77: OP_2(bit, 6, REG_A);             // BIT 6,A      2   8   Z 0 1 - 
case 0x78: OP_2(bit, 7, REG_B);             // BIT 7,B      2   8   Z 0 1 - 
case 0x79: OP_2(bit, 7, REG_C);             // BIT 7,C      2   8   Z 0 1 - 
case 0x7A: OP_2(bit, 7, REG_D);             // BIT 7,D      2   8   Z 0 1 - 
case 0x7B: OP_2(bit, 7, REG_E);             // BIT 7,E      2   8   Z 0 1 - 
case 0x7C: OP_2(bit, 7, REG_H);             // BIT 7,H      2   8   Z 0 1 - 
case 0x7D: OP_2(bit, 7, REG_L);             // BIT 7,L      2   8   Z 0 1 - 
case 0x7E: OP_1(biti, 7);                   // BIT 7,(HL)   2  16   Z 0 1 - 
case 0x7F: OP_2(bit, 7, REG_A);             // BIT 7,A      2   8   Z 0 1 -
case 0x80: OP_2(res, 0, REG_B);             // RES 0,B      2   8   - - - -    
case 0x81: OP_2(res, 0, REG_C);             // RES 0,C      2   8   - - - -    
case 0x82: OP_2(res, 0, REG_D);             // RES 0,D      2   8   - - - -    
case 0x83: OP_2(res, 0, REG_E);             // RES 0,E      2   8   - - - -    
case 0x84: OP_2(res, 0, REG_H);             // RES 0,H      2   8   - - - -    
case 0x85: OP_2(res, 0, REG_L);             // RES 0,L      2   8   - - - -    
case 0x86: OP_1(resi, 0);                   // RES 0,(HL)   2  16   - - - -    
case 0x87: OP_2(res, 0, REG_A);             // RES 0,A      2   8   - - - -    
case 0x88: OP_2(res, 1, REG_B);             // RES 1,B      2   8   - - - -    
case 0x89: OP_2(res, 1, REG_C);             // RES 1,C      2   8   - - - -    
case 0x8A: OP_2(res, 1, REG_D);             // RES 1,D      2   8   - - - -    
case 0x8B: OP_2(res, 1, REG_E);             // RES 1,E      2   8   - - - -    
case 0x8C: OP_2(res, 1, REG_H);             // RES 1,H      2   8   - - - -    
case 0x8D: OP_2(res, 1, REG_L);             // RES 1,L      2   8   - - - -    
case 0x8E: OP_1(resi, 1);                   // RES 1,(HL)   2  16   - - - -    
case 0x8F: OP_2(res, 1, REG_A);             // RES 1,A      2   8   - - - -
case 0x90: OP_2(res, 2, REG_B);             // RES 2,B      2   8   - - - -   
case 0x91: OP_2(res, 2, REG_C);             // RES 2,C      2   8   - - - -   
case 0x92: OP_2(res, 2, REG_D);             // RES 2,D      2   8   - - - -   
case 0x93: OP_2(res, 2, REG_E);             // RES 2,E      2   8   - - - -   
case 0x94: OP_2(res, 2, REG_H);             // RES 2,H      2   8   - - - -   
case 0x95: OP_2(res, 2, REG_L);             // RES 2,L      2   8   - - - -   
case 0x96: OP_1(resi, 2);                   // RES 2,(HL)   2  16   - - - -   
case 0x97: OP_2(res, 2, REG_A);             // RES 2,A      2   8   - - - -   
case 0x98: OP_2(res, 3, REG_B);             // RES 3,B      2   8   - - - -   
case 0x99: OP_2(res, 3, REG_C);             // RES 3,C      2   8   - - - -   
case 0x9A: OP_2(res, 3, REG_D);             // RES 3,D      2   8   - - - -   
case 0x9B: OP_2(res, 3, REG_E);             // RES 3,E      2   8   - - - -   
case 0x9C: OP_2(res, 3, REG_H);             // RES 3,H      2   8   - - - -   
case 0x9D: OP_2(res, 3, REG_L);             // RES 3,L      2   8   - - - -   
case 0x9E: OP_1(resi, 3);                   // RES 3,(HL)   2  16   - - - -   
case 0x9F: OP_2(res, 3, REG_A);             // RES 3,A      2   8   - - - -
case 0xA0: OP_2(res, 4, REG_B);             // RES 4,B      2   8   - - - -  
case 0xA1: OP_2(res, 4, REG_C);             // RES 4,C      2   8   - - - -  
case 0xA2: OP_2(res, 4, REG_D);             // RES 4,D      2   8   - - - -  
case 0xA3: OP_2(res, 4, REG_E);             // RES 4,E      2   8   - - - -  
case 0xA4: OP_2(res, 4, REG_H);             // RES 4,H      2   8   - - - -  
case 0xA5: OP_2(res, 4, REG_L);             // RES 4,L      2   8   - - - -  
case 0xA6: OP_1(resi, 4);                   // RES 4,(HL)   2  16   - - - -  
case 0xA7: OP_2(res, 4, REG_A);             // RES 4,A      2   8   - - - -  
case 0xA8: OP_2(res, 5, REG_B);             // RES 5,B      2   8   - - - -  
case 0xA9: OP_2(res, 5, REG_C);             // RES 5,C      2   8   - - - -  
case 0xAA: OP_2(res, 5, REG_D);             // RES 5,D      2   8   - - - -  
case 0xAB: OP_2(res, 5, REG_E);             // RES 5,E      2   8   - - - -  
case 0xAC: OP_2(res, 5, REG_H);             // RES 5,H      2   8   - - - -  
case 0xAD: OP_2(res, 5, REG_L);             // RES 5,L      2   8   - - - -  
case 0xAE: OP_1(resi, 5);                   // RES 5,(HL)   2  16   - - - -  
case 0xAF: OP_2(res, 5, REG_A);             // RES 5,A      2   8   - - - -
case 0xB0: OP_2(res, 6, REG_B);             // RES 6,B      2   8   - - - - 
case 0xB1: OP_2(res, 6, REG_C);             // RES 6,C      2   8   - - - - 
case 0xB2: OP_2(res, 6, REG_D);             // RES 6,D      2   8   - - - - 
case 0xB3: OP_2(res, 6, REG_E);             // RES 6,E      2   8   - - - - 
case 0xB4: OP_2(res, 6, REG_H);             // RES 6,H      2   8   - - - - 
case 0xB5: OP_2(res, 6, REG_L);             // RES 6,L      2   8   - - - - 
case 0xB6: OP_1(resi, 6);                   // RES 6,(HL)   2  16   - - - - 
case 0xB7: OP_2(res, 6, REG_A);             // RES 6,A      2   8   - - - - 
case 0xB8: OP_2(res, 7, REG_B);             // RES 7,B      2   8   - - - - 
case 0xB9: OP_2(res, 7, REG_C);             // RES 7,C      2   8   - - - - 
case 0xBA: OP_2(res, 7, REG_D);             // RES 7,D      2   8   - - - - 
case 0xBB: OP_2(res, 7, REG_E);             // RES 7,E      2   8   - - - - 
case 0xBC: OP_2(res, 7, REG_H);             // RES 7,H      2   8   - - - - 
case 0xBD: OP_2(res, 7, REG_L);             // RES 7,L      2   8   - - - - 
case 0xBE: OP_1(resi, 7);                   // RES 7,(HL)   2  16   - - - - 
case 0xBF: OP_2(res, 7, REG_A);             // RES 7,A      2   8   - - - -
case 0xC0: OP_2(set, 0, REG_B);             // SET 0,B      2   8   - - - -    
case 0xC1: OP_2(set, 0, REG_C);             // SET 0,C      2   8   - - - -    
case 0xC2: OP_2(set, 0, REG_D);             // SET 0,D      2   8   - - - -    
case 0xC3: OP_2(set, 0, REG_E);             // SET 0,E      2   8   - - - -    
case 0xC4: OP_2(set, 0, REG_H);             // SET 0,H      2   8   - - - -    
case 0xC5: OP_2(set, 0, REG_L);             // SET 0,L      2   8   - - - -    
case 0xC6: OP_1(seti, 0);                   // SET 0,(HL)   2  16   - - - -    
case 0xC7: OP_2(set, 0, REG_A);             // SET 0,A      2   8   - - - -    
case 0xC8: OP_2(set, 1, REG_B);             // SET 1,B      2   8   - - - -    
case 0xC9: OP_2(set, 1, REG_C);             // SET 1,C      2   8   - - - -    
case 0xCA: OP_2(set, 1, REG_D);             // SET 1,D      2   8   - - - -    
case 0xCB: OP_2(set, 1, REG_E);             // SET 1,E      2   8   - - - -    
case 0xCC: OP_2(set, 1, REG_H);             // SET 1,H      2   8   - - - -    
case 0xCD: OP_2(set, 1, REG_L);             // SET 1,L      2   8   - - - -    
case 0xCE: OP_1(seti, 1);                   // SET 1,(HL)   2  16   - - - -    
case 0xCF: OP_2(set, 1, REG_A);             // SET 1,A      2   8   - - - -
case 0xD0: OP_2(set, 2, REG_B);             // SET 2,B      2   8   - - - -   
case 0xD1: OP_2(set, 2, REG_C);             // SET 2,C      2   8   - - - -   
case 0xD2: OP_2(set, 2, REG_D);             // SET 2,D      2   8   - - - -   
case 0xD3: OP_2(set, 2, REG_E);             // SET 2,E      2   8   - - - -   
case 0xD4: OP_2(set, 2, REG_H);             // SET 2,H      2   8   - - - -   
case 0xD5: OP_2(set, 2, REG_L);             // SET 2,L      2   8   - - - -   
case 0xD6: OP_1(seti, 2);                   // SET 2,(HL)   2  16   - - - -   
case 0xD7: OP_2(set, 2, REG_A);             // SET 2,A      2   8   - - - -   
case 0xD8: OP_2(set, 3, REG_B);             // SET 3,B      2   8   - - - -   
case 0xD9: OP_2(set, 3, REG_C);             // SET 3,C      2   8   - - - -   
case 0xDA: OP_2(set, 3, REG_D);             // SET 3,D      2   8   - - - -   
case 0xDB: OP_2(set, 3, REG_E);             // SET 3,E      2   8   - - - -   
case 0xDC: OP_2(set, 3, REG_H);             // SET 3,H      2   8   - - - -   
case 0xDD: OP_2(set, 3, REG_L);             // SET 3,L      2   8   - - - -   
case 0xDE: OP_1(seti, 3);                   // SET 3,(HL)   2  16   - - - -   
case 0xDF: OP_2(set, 3, REG_A);             // SET 3,A      2   8   - - - -
case 0xE0: OP_2(set, 4, REG_B);             // SET 4,B      2   8   - - - -  
case 0xE1: OP_2(set, 4, REG_C);             // SET 4,C      2   8   - - - -  
case 0xE2: OP_2(set, 4, REG_D);             // SET 4,D      2   8   - - - -  
case 0xE3: OP_2(set, 4, REG_E);             // SET 4,E      2   8   - - - -  
case 0xE4: OP_2(set, 4, REG_H);             // SET 4,H      2   8   - - - -  
case 0xE5: OP_2(set, 4, REG_L);             // SET 4,L      2   8   - - - -  
case 0xE6: OP_1(seti, 4);                   // SET 4,(HL)   2  16   - - - -  
case 0xE7: OP_2(set, 4, REG_A);             // SET 4,A      2   8   - - - -  
case 0xE8: OP_2(set, 5, REG_B);             // SET 5,B      2   8   - - - -  
case 0xE9: OP_2(set, 5, REG_C);             // SET 5,C      2   8   - - - -  
case 0xEA: OP_2(set, 5, REG_D);             // SET 5,D      2   8   - - - -  
case 0xEB: OP_2(set, 5, REG_E);             // SET 5,E      2   8   - - - -  
case 0xEC: OP_2(set, 5, REG_H);             // SET 5,H      2   8   - - - -  
case 0xED: OP_2(set, 5, REG_L);             // SET 5,L      2   8   - - - -  
case 0xEE: OP_1(seti, 5);                   // SET 5,(HL)   2  16   - - - -  
case 0xEF: OP_2(set, 5, REG_A);             // SET 5,A      2   8   - - - -
case 0xF0: OP_2(set, 6, REG_B);             // SET 6,B      2   8   - - - - 
case 0xF1: OP_2(set, 6, REG_C);             // SET 6,C      2   8   - - - - 
case 0xF2: OP_2(set, 6, REG_D);             // SET 6,D      2   8   - - - - 
case 0xF3: OP_2(set, 6, REG_E);             // SET 6,E      2   8   - - - - 
case 0xF4: OP_2(set, 6, REG_H);             // SET 6,H      2   8   - - - - 
case 0xF5: OP_2(set, 6, REG_L);             // SET 6,L      2   8   - - - - 
case 0xF6: OP_1(seti, 6);                   // SET 6,(HL)   2  16   - - - - 
case 0xF7: OP_2(set, 6, REG_A);             // SET 6,A      2   8   - - - - 
case 0xF8: OP_2(set, 7, REG_B);             // SET 7,B      2   8   - - - - 
case 0xF9: OP_2(set, 7, REG_C);             // SET 7,C      2   8   - - - - 
case 0xFA: OP_2(set, 7, REG_D);             // SET 7,D      2   8   - - - - 
case 0xFB: OP_2(set, 7, REG_E);             // SET 7,E      2   8   - - - - 
case 0xFC: OP_2(set, 7, REG_H);             // SET 7,H      2   8   - - - - 
case 0xFD: OP_2(set, 7, REG_L);             // SET 7,L      2   8   - - - - 
case 0xFE: OP_1(seti, 7);                   // SET 7,(HL)   2  16   - - - - 
case 0xFF: OP_2(set, 7, REG_A);             // SET 7,A      2   8   - - - -
}

