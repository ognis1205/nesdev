/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CPU_H_
#define _NES_CPU_H_
#include <string>
#include <vector>

#include "types.h"

namespace nes {
namespace bus {

class Bus;

}  // namespace bus
}  // namespace nes

namespace nes {
namespace cpu {

/*
 * RICOH version MOS Technology 6502.
 * SEE: http://www.6502.org/
 */
class MOS6502 {
 public:
  static const types::uint8_t kInitialStackAddress = 0xFD;
  static const types::uint16_t kStackBaseAddress = 0x0100;
  static const types::uint16_t kResetVectorAddress = 0xFFFC;
  static const types::uint16_t kNMIVectorAddress = 0xFFFA;
  
  MOS6502();
  ~MOS6502();

  union {
    types::uint8_t REGISTER;
  } A;
  union {
    types::uint8_t REGISTER;
  } X;
  union {
    types::uint8_t REGISTER;
  } Y;
  union {
    types::uint8_t REGISTER;
  } S;
  union {
    types::uint16_t REGISTER;
    types::Bitfield<0, 8, types::uint16_t> OFFSET;
    types::Bitfield<8, 8, types::uint16_t> PAGE;
  } PC;
  union {
    types::uint8_t REGISTER;
    types::Bitfield<0, 1, types::uint8_t> CARRY;
    types::Bitfield<1, 1, types::uint8_t> ZERO;
    types::Bitfield<2, 1, types::uint8_t> IRQ_DISABLE;
    types::Bitfield<3, 1, types::uint8_t> DECIMAL_MODE;
    types::Bitfield<4, 1, types::uint8_t> BRK_COMMAND;
    types::Bitfield<5, 1, types::uint8_t> UNUSED;
    types::Bitfield<6, 1, types::uint8_t> OVERFLOW;
    types::Bitfield<7, 1, types::uint8_t> NEGATIVE;
  } P;

  void HandleReset();
  void HandleIRQ();
  void HandleNMI();
  void HandleClock();
  void ConnectTo(bus::Bus* bus);
  types::uint8_t Read(types::uint16_t address);
  void Write(types::uint16_t address, types::uint8_t data);
  types::uint8_t Fetch();

 private:
  struct Instruction {
    std::string name;
    types::uint8_t (MOS6502::*addressing_mode)(void);
    types::uint8_t (MOS6502::*opcode)(void);
    types::uint8_t cycles;
  };

  // Assisstive: Linkage to the communications bus.
  bus::Bus* bus_;
  // Assisstive: Lookup table of opcodes.
  std::vector<Instruction> lookup_;
  // Assisstive: Instruction byte.
  types::uint8_t opcode_;
  // Assisstive: Working input value to the ALU.
  types::uint8_t operand_;
  // Assisstive: Currently handling absolute mode memory address.
  types::uint16_t address_;
  // Assisstive: Used for relative addressing.
  types::uint16_t offset_;
  // Assisstive: Count how many cycles the instruction has remaining.
  types::uint32_t cycles_delayed_;
  // Assisstive: A global accumulation of the number of clocks.
  types::uint32_t clock_count_;

  // Addressing Modes.
  types::uint8_t IMP();
  types::uint8_t IMM();
  types::uint8_t ZP0();
  types::uint8_t ZPX();
  types::uint8_t ZPY();
  types::uint8_t REL();
  types::uint8_t ABS();
  types::uint8_t ABX();
  types::uint8_t ABY();
  types::uint8_t IND();
  types::uint8_t IZX();
  types::uint8_t IZY();

  // Opcodes.
  types::uint8_t ADC();
  types::uint8_t AND();
  types::uint8_t ASL();
  types::uint8_t BCC();
  types::uint8_t BCS();
  types::uint8_t BEQ();
  types::uint8_t BIT();
  types::uint8_t BMI();
  types::uint8_t BNE();
  types::uint8_t BPL();
  types::uint8_t BRK();
  types::uint8_t BVC();
  types::uint8_t BVS();
  types::uint8_t CLC();
  types::uint8_t CLD();
  types::uint8_t CLI();
  types::uint8_t CLV();
  types::uint8_t CMP();
  types::uint8_t CPX();
  types::uint8_t CPY();
  types::uint8_t DEC();
  types::uint8_t DEX();
  types::uint8_t DEY();
  types::uint8_t EOR();
  types::uint8_t INC();
  types::uint8_t INX();
  types::uint8_t INY();
  types::uint8_t JMP();
  types::uint8_t JSR();
  types::uint8_t LDA();
  types::uint8_t LDX();
  types::uint8_t LDY();
  types::uint8_t LSR();
  types::uint8_t NOP();
  types::uint8_t ORA();
  types::uint8_t PHA();
  types::uint8_t PHP();
  types::uint8_t PLA();
  types::uint8_t PLP();
  types::uint8_t ROL();
  types::uint8_t ROR();
  types::uint8_t RTI();
  types::uint8_t RTS();
  types::uint8_t SBC();
  types::uint8_t SEC();
  types::uint8_t SED();
  types::uint8_t SEI();
  types::uint8_t STA();
  types::uint8_t STX();
  types::uint8_t STY();
  types::uint8_t TAX();
  types::uint8_t TAY();
  types::uint8_t TSX();
  types::uint8_t TXA();
  types::uint8_t TXS();
  types::uint8_t TYA();
  types::uint8_t XXX();
};

}  // namespace cpu
}  // namespace nes
#endif
