/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_OPCODES_H_
#define _NES_CORE_OPCODES_H_
#include "types.h"

namespace nes {
namespace core {

/*
 * NOTE:
 * Most instructions that explicitly reference memory locations have bit
 * patterns of the form aaabbbcc. The aaa and cc bits determine the opcode,
 * and the bbb bits determine the addressing mode.
 * The conditional branch instructions all have the form xxy10000.
 * The flag indicated by xx is compared with y, and the branch is taken if they are equal.
 * SEE: http://nparker.llx.com/a2/opcodes.html
 */
  union Opcode {
    Byte value;
    Bitfield<0, 2, Byte> cc;
    Bitfield<2, 3, Byte> bbb;
    Bitfield<5, 3, Byte> aaa;
    Bitfield<0, 5, Byte> zzzzz;
    Bitfield<5, 1, Byte> y;
    Bitfield<6, 2, Byte> xx;
  };

  enum class MemoryAccess {
    READ,
    WRITE,
    READWRITE
  };

  enum class OpcodeTypeMask : Byte {
    CC01 = 0b00000001,
    CC10 = 0b00000010,
    CC00 = 0b00000000,
    CC11 = 0b00000011, // No instructions defined
    XXY0 = 0b00000000,
    XXY1 = 0b00010000
  };

  enum class AddressingModeMask : Byte {
    BBB000 = 0b00000000, // CC01 -> ZPX      / CC10 -> IMM      / CC00 -> IMM      /
    BBB001 = 0b00000100, // CC01 -> ZP0      / CC10 -> ZP0      / CC00 -> ZP0      /
    BBB010 = 0b00001000, // CC01 -> IMM      / CC10 -> ACC      / CC00 -> ---      /
    BBB011 = 0b00001100, // CC01 -> ABS      / CC10 -> ABS      / CC00 -> ABS      /
    BBB100 = 0b00010000, // CC01 -> ZPY      / CC10 -> ---      / CC00 -> ---      /
    BBB101 = 0b00010100, // CC01 -> ZPX      / CC10 -> ZPX      / CC00 -> ZPX      /
    BBB110 = 0b00011000, // CC01 -> ABY      / CC10 -> ---      / CC00 -> ---      /
    BBB111 = 0b00011100, // CC01 -> ABX      / CC10 -> ABX      / CC00 -> ABX      /
  };

  enum class InstructionMask : Byte {
    AAA000 = 0b00000000, // CC01 -> ORA      / CC10 -> ASL      / CC00 -> ---      /
    AAA001 = 0b00100000, // CC01 -> AND      / CC10 -> ROL      / CC00 -> BIT      /
    AAA010 = 0b01000000, // CC01 -> EOR      / CC10 -> LSR      / CC00 -> JMP      /
    AAA011 = 0b01100000, // CC01 -> ADC      / CC10 -> ROR      / CC00 -> JMP(ABS) /
    AAA100 = 0b10000000, // CC01 -> STA      / CC10 -> STX      / CC00 -> STY      /
    AAA101 = 0b10100000, // CC01 -> LDA      / CC10 -> LDX      / CC00 -> LDY      /
    AAA110 = 0b11000000, // CC01 -> CMP      / CC10 -> DEC      / CC00 -> CPY      /
    AAA111 = 0b11100000  // CC01 -> SBC      / CC10 -> INC      / CC00 -> CPX      /
  };

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_OPCODES_H_
