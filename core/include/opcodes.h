/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_OPCODES_H_
#define _NES_CORE_OPCODES_H_
#include <functional>
#include <string>
#include "pipeline.h"
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
  union OpcodeByte {
    Byte value;
    Bitfield<0, 2, Byte> cc;
    Bitfield<2, 3, Byte> bbb;
    Bitfield<5, 3, Byte> aaa;
    Bitfield<0, 5, Byte> zzzzz;
    Bitfield<5, 1, Byte> y;
    Bitfield<6, 2, Byte> xx;
  };

  enum class OpcodeTypeBit : Byte {
    CC01 = 0b00000001,
    CC10 = 0b00000010,
    CC00 = 0b00000000,
    CC11 = 0b00000011, // *** N/A (not on 6502 architecture) ***
    XXY0 = 0b00000000,
    XXY1 = 0b00010000
  };

  enum class AddressingModeBit : Byte {
    BBB000 = 0b00000000, // CC01 -> ZPX      / CC10 -> IMM      / CC00 -> IMM      /
    BBB001 = 0b00000100, // CC01 -> ZP0      / CC10 -> ZP0      / CC00 -> ZP0      /
    BBB010 = 0b00001000, // CC01 -> IMM      / CC10 -> ACC      / CC00 -> ---      /
    BBB011 = 0b00001100, // CC01 -> ABS      / CC10 -> ABS      / CC00 -> ABS      /
    BBB100 = 0b00010000, // CC01 -> ZPY      / CC10 -> ---      / CC00 -> ---      /
    BBB101 = 0b00010100, // CC01 -> ZPX      / CC10 -> ZPX      / CC00 -> ZPX      /
    BBB110 = 0b00011000, // CC01 -> ABY      / CC10 -> ---      / CC00 -> ---      /
    BBB111 = 0b00011100, // CC01 -> ABX      / CC10 -> ABX      / CC00 -> ABX      /
  };

  enum class InstructionBit : Byte {
    AAA000 = 0b00000000, // CC01 -> ORA      / CC10 -> ASL      / CC00 -> ---      /
    AAA001 = 0b00100000, // CC01 -> AND      / CC10 -> ROL      / CC00 -> BIT      /
    AAA010 = 0b01000000, // CC01 -> EOR      / CC10 -> LSR      / CC00 -> JMP      /
    AAA011 = 0b01100000, // CC01 -> ADC      / CC10 -> ROR      / CC00 -> JMP(ABS) /
    AAA100 = 0b10000000, // CC01 -> STA      / CC10 -> STX      / CC00 -> STY      /
    AAA101 = 0b10100000, // CC01 -> LDA      / CC10 -> LDX      / CC00 -> LDY      /
    AAA110 = 0b11000000, // CC01 -> CMP      / CC10 -> DEC      / CC00 -> CPY      /
    AAA111 = 0b11100000  // CC01 -> SBC      / CC10 -> INC      / CC00 -> CPX      /
  };

  enum class ProcessorFlagBit : Byte {
    XX00 = 0b00000000, // Negative
    XX01 = 0b01000000, // Overflow
    XX10 = 0b10000000, // Carry
    XX11 = 0b11000000  // Zero
  };

  enum class AddressingMode {
    INVALID, // Invalid (not official)
    ABS,     // Absolute
    ABX,     // Absolute, X-indexed
    ABY,     // Absolute, Y-indexed
    ACC,     // Accumulator
    IMM,     // Immediate
    IMP,     // Implied
    IND,     // Indirect
    IZX,     // Indirect, X-indexed
    IZY,     // Indirect, Y-indexed
    REL,     // Relative
    ZP0,     // Zeropage
    ZPX,     // Zeropage, X-indexed
    ZPY      // Zeropage, Y-indexed
  };

  enum class Instruction {
    INVALID, // Invalid (not official)
    ADC,     // Add with carry
    AND,     // And (with accumulator)
    ASL,     // Arithmetic shift left
    BCC,     // Branch on carry clear
    BCS,     // Branch on carry set
    BEQ,     // Branch on equal (zero set)
    BIT,     // Bit test
    BMI,     // Branch on minus (negative set)
    BNE,     // Branch on not equal (zero clear)
    BPL,     // Branch on plus (negative clear)
    BRK,     // Break/Interrupt
    BVC,     // Branch on overflow clear
    BVS,     // Branch on overflow set
    CLC,     // Clear carry
    CLD,     // Clear decimal
    CLI,     // Clear interrupt disable
    CLV,     // Clear overflow
    CMP,     // Compare (with accumulator)
    CPX,     // Compare with X
    CPY,     // Compare with Y
    DCP,     // *** N/A (not on 6502 architecture) ***
    DEC,     // Decrement
    DEX,     // Decrement X
    DEY,     // Decrement Y    
    EOR,     // Exclusive or (with accumulator)
    INC,     // Increment
    INX,     // Increment X
    INY,     // Increment Y
    ISB,     // *** N/A (not on 6502 architecture) ***
    JMP,     // Jump
    JSR,     // Jump subroutine
    LAX,     // *** N/A (not on 6502 architecture) ***
    LDA,     // Load accumulator
    LDX,     // Load X
    LDY,     // Load Y
    LSR,     // Logical shift right
    NOP,     // No operation
    ORA,     // Or with accumulator
    PHA,     // Push accumulator
    PHP,     // Push processor status (SR)
    PLA,     // Pull accumulator
    PLP,     // Pull processor status (SR)
    RLA,     // *** N/A (not on 6502 architecture) ***
    ROL,     // Rotate left
    ROR,     // Rotate right
    RRA,     // *** N/A (not on 6502 architecture) ***
    RTI,     // Return from interrupt
    RTS,     // Return from subroutine
    SAX,     // *** N/A (not on 6502 architecture) ***
    SBC,     // Subtract with carry
    SEC,     // Set carry
    SED,     // Set decimal
    SEI,     // Set interrupt disable
    SLO,     // *** N/A (not on 6502 architecture) ***
    SRE,     // *** N/A (not on 6502 architecture) ***
    STA,     // Store accumulator
    STX,     // Store X
    STY,     // Store Y
    TAX,     // Transfer accumulator to X
    TAY,     // Transfer accumulator to Y
    TSX,     // Transfer stack pointer to X
    TXA,     // Transfer X to accumulator
    TXS,     // Transfer X to stack pointer
    TYA      // Transfer Y to accumulator
  };

  struct Opcode {
    AddressingMode addressing_mode;
    Instruction instruction;
  };

  enum class MemoryAccess {
    READ,
    WRITE,
    READWRITE
  };

  class OpcodeHandler {
    // Instruction/Addressing mode: No-operation (not official)
    virtual Pipeline NOP() const noexcept = 0;
    // Instruction: Non-maskable interruption
    virtual Pipeline InstructNMI() noexcept = 0;

    // Addressing mode: Absolute
    virtual Pipeline AddressABS(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Absolute, X-indexed
    virtual Pipeline AddressABX(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Absolute, Y-indexed
    virtual Pipeline AddressABY(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Accumulator
    virtual Pipeline AddressACC(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Immediate
    virtual Pipeline AddressIMM(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Implied
    virtual Pipeline AddressIMP(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Indirect
    virtual Pipeline AddressIND(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Indirect, X-indexed
    virtual Pipeline AddressIZX(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Indirect, Y-indexed
    virtual Pipeline AddressIZY(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Relative
    virtual Pipeline AddressREL(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Zeropage
    virtual Pipeline AddressZP0(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Zeropage, X-indexed
    virtual Pipeline AddressZPX(const Opcode& opcode) noexcept = 0;
    // Addressing mode: Zeropage, Y-indexed
    virtual Pipeline AddressZPY(const Opcode& opcode) noexcept = 0;

    // Instruction: Add
    //virtual Pipeline InstructADC(const Opcode& opcode) noexcept = 0;
    // Instruction: Add
    virtual Pipeline InstructADD(const Opcode& opcode) noexcept = 0;
    // Instruction: And
    virtual Pipeline InstructAND(const Opcode& opcode) noexcept = 0;
    // Instruction: Branch
    virtual Pipeline InstructBRA(const std::function<bool()>& condition) noexcept = 0;
    // Instruction: Compare
    virtual Pipeline InstructCMP(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructDCP(const Opcode& opcode) noexcept = 0;
    // Instruction: Decrement
    virtual Pipeline InstructDEC(const Opcode& opcode) noexcept = 0;
    // Instruction: Exclusive or
    virtual Pipeline InstructEOR(const Opcode& opcode) noexcept = 0;
    // Instruction: Increment
    virtual Pipeline InstructINC(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructISB(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructLAX(const Opcode& opcode) noexcept = 0;
    // Instruction: Load
    virtual Pipeline InstructLOD(const Opcode& opcode) noexcept = 0;
    // Instruction: Left shift
    virtual Pipeline InstructLSH(const Opcode& opcode, const bool& shift_in_carry) noexcept = 0;
    // Instruction: Or
    virtual Pipeline InstructORA(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructRLA(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructRRA(const Opcode& opcode) noexcept = 0;
    // Instruction: Right shift
    virtual Pipeline InstructRSH(const Opcode& opcode, const bool& shift_in_carry) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructSAX(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructSLO(const Opcode& opcode) noexcept = 0;
    // Instruction: *** N/A ***
    [[maybe_unused]] virtual Pipeline InstructSRE(const Opcode& opcode) noexcept = 0;
    // Instruction: Store
    virtual Pipeline InstructSTR(const Opcode& opcode) noexcept = 0;
    // Instruction: Subtract
    virtual Pipeline InstructSUB(const Opcode& opcode) noexcept = 0;
  };

  [[nodiscard]] Opcode Decode(const OpcodeByte& opcode) noexcept;

  [[nodiscard]] MemoryAccess GetMemoryAccess(const Instruction& instruction) noexcept;

  [[nodiscard]] std::string_view ToString(const Instruction& instruction) noexcept;

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_OPCODES_H_
