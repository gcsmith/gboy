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

// Joypad Input

#define PORT_JOYP       0x00    // joypad (R/W)

#define JOYP_SEL_DIR    0x10
#define JOYP_SEL_BTN    0x20

// Serial Data Transfer

#define PORT_SB         0x01    // serial transfer data (R/W)
#define PORT_SC         0x02    // serial transfer control (R/W)

#define SC_CLK_SHIFT    0x01    // shift clock source
#define SC_CLK_SPEED    0x02    // CGB only, clock speed
#define SC_XFER_START   0x80    // transfer start flag

// Timing and Divider Registers

#define PORT_DIV        0x04    // divider register (R/W)
#define PORT_TIMA       0x05    // timer counter (R/W)
#define PORT_TMA        0x06    // timer modulo (R/W)
#define PORT_TAC        0x07    // timer control (R/W)

#define TAC_ENABLED     0x04
#define TAC_SEL_MASK    0x03

// Interrupts

#define PORT_IF         0x0F    // interrupt flag (R/W)
#define PORT_IE         0xFF    // interrupt enable (R/W)

#define INT_VBLANK      0x01    // vertical blanking
#define INT_LCDSTAT     0x02    // LCDC stat
#define INT_TIMER       0x04    // timer overflow
#define INT_SERIAL      0x08    // serial transfer completion
#define INT_JOYPAD      0x10    // P10-P13 edge / joypad
#define INT_MASK        0x1F

#define INT_VEC_VBLANK  0x40    // interrupt address for VBLANK
#define INT_VEC_LCDSTAT 0x48    // interrupt address for LCDSTAT
#define INT_VEC_TIMER   0x50    // interrupt address for TIMER
#define INT_VEC_SERIAL  0x58    // interrupt address for SERIAL
#define INT_VEC_JOYPAD  0x60    // interrupt address for JOYPAD

// LCD control, position, and scrolling

#define PORT_LCDC       0x40    // LCD control (R/W)
#define PORT_STAT       0x41    // LCDC status (R/W)
#define PORT_SCY        0x42    // scroll Y (R/W)
#define PORT_SCX        0x43    // scroll X (R/W)
#define PORT_LY         0x44    // LCDC Y-coordinate (R)
#define PORT_LYC        0x45    // LY compare (R/W)
#define PORT_DMA        0x46    // DMA transfer and start address (W)
#define PORT_BGP        0x47    // background palette data (R/W)
#define PORT_OBP0       0x48    // object palette 0 data (R/W)
#define PORT_OBP1       0x49    // object palette 1 data (R/W)
#define PORT_WY         0x4A    // window Y position (R/W)
#define PORT_WX         0x4B    // window X position minus 7 (R/W)

// CGB Registers

#define PORT_KEY1       0x4D    // prepare CPU speed switch

#define KEY1_PREP       0x01    // prepare speed switch
#define KEY1_SPEED      0x80    // current speed

#define PORT_VBK        0x4F    // VRAM bank specification
#define PORT_HDMA1      0x51    // new DMA source, hi
#define PORT_HDMA2      0x52    // new DMA source, lo
#define PORT_HDMA3      0x53    // new DMA destination, hi
#define PORT_HDMA4      0x54    // new DMA destination, lo
#define PORT_HDMA5      0x55    // new DMA length/mode/start

#define PORT_RP         0x56    // infrared communications port

#define RP_WR_DATA      0x01    // write data - enable or disable LED
#define RR_RD_DATA      0x02    // read data - 0 if receiving, 1 if normal
#define RP_RD_ENABLE    0xC0    // data read enable - 00 disabled, 11 enabled

#define PORT_BCPS       0x68    // background color palette specification
#define PORT_BCPD       0x69    // background color palette data
#define PORT_OCPS       0x6A    // object color palette specification
#define PORT_OCPD       0x6B    // object color palette data
#define PORT_SVBK       0x70    // WRAM bank specification

// Sound Channel 1 - Tone & Sweep

#define PORT_NR10       0x10    // channel 1 sweep register
#define PORT_NR11       0x11    // channel 1 sound length/wave pattern duty
#define PORT_NR12       0x12    // channel 1 volume envelope
#define PORT_NR13       0x13    // channel 1 frequency lo
#define PORT_NR14       0x14    // channel 1 frequency hi

// Sound Channel 2 - Tone

#define PORT_NR21       0x16    // channel 2 sound length/wave pattern duty
#define PORT_NR22       0x17    // channel 2 volume envelope
#define PORT_NR23       0x18    // channel 2 frequency lo
#define PORT_NR24       0x19    // channel 2 frequency hi

// Sound Channel 3 - Wave Output

#define PORT_NR30       0x1A    // channel 3 sound on/off
#define PORT_NR31       0x1B    // channel 3 sound length
#define PORT_NR32       0x1C    // channel 3 select output level
#define PORT_NR33       0x1D    // channel 3 frequency lo
#define PORT_NR34       0x1E    // channel 3 frequency hi
#define PORT_WAV0       0x30    // channel 3 wave pattern (32 4-bit samples)
#define PORT_WAV1       (PORT_WAV0 + 1)
#define PORT_WAV2       (PORT_WAV0 + 2)
#define PORT_WAV3       (PORT_WAV0 + 3)
#define PORT_WAV4       (PORT_WAV0 + 4)
#define PORT_WAV5       (PORT_WAV0 + 5)
#define PORT_WAV6       (PORT_WAV0 + 6)
#define PORT_WAV7       (PORT_WAV0 + 7)
#define PORT_WAV8       (PORT_WAV0 + 8)
#define PORT_WAV9       (PORT_WAV0 + 9)
#define PORT_WAVA       (PORT_WAV0 + 10)
#define PORT_WAVB       (PORT_WAV0 + 11)
#define PORT_WAVC       (PORT_WAV0 + 12)
#define PORT_WAVD       (PORT_WAV0 + 13)
#define PORT_WAVE       (PORT_WAV0 + 14)
#define PORT_WAVF       (PORT_WAV0 + 15)

// Sound Channel 4 - Noise

#define PORT_NR41       0x20    // channel 4 sound length
#define PORT_NR42       0x21    // channel 4 volume envelope
#define PORT_NR43       0x22    // channel 4 polynomial counter
#define PORT_NR44       0x23    // channel 4 counter

// Sound Control Registers

#define PORT_NR50       0x24    // channel control / on-off / volume
#define PORT_NR51       0x25    // select sound output terminal
#define PORT_NR52       0x26    // sound on/off

// Undocumented Registers

#define PORT_BIOS       0x50    // enable/disable bios page
#define PORT_UNK_6C     0x6C    // 0xFE, CGB only, 0xFF in non-CGB mode
#define PORT_UNK_72     0x72    // 0x00
#define PORT_UNK_73     0x73    // 0x00
#define PORT_UNK_74     0x74    // 0x00, CGB only, 0xFF in non-CGB mode
#define PORT_UNK_75     0x75    // 0x8F
#define PORT_UNK_76     0x76    // 0x00, read-only
#define PORT_UNK_77     0x77    // 0x00, read-only

