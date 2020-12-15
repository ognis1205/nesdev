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

/* http://www.6502.org/ */
class CPU6502 {
 public:
  CPU6502();
  ~CPU6502();

  // Accumulator.
  types::uint8_t A = 0x00;
  // X register.
  types::uint8_t X = 0x00;
  // Y register.
  types::uint8_t Y = 0x00;
  // Stack pointer.
  types::uint8_t S = 0x00;
  // Program counter.
  types::uint16_t PC = 0x0000;
  // Status register.
  types::uint8_t P = 0x00;

  // Status register masks.
  enum Status {
    CARRY = (1u << 0),
    ZERO = (1u << 1),
    IRQ_DISABLE = (1u << 2),
    DECIMAL_MODE = (1u << 3),
    BRK_COMMAND = (1u << 4),
    UNUSED = (1u << 5),
    OVERFLOW = (1u << 6),
    NEGATIVE = (1u << 7)
  };

  // Reset interrupt.
  void HandleReset();
  // Interrupt request.
  void HandleIRQ();
  // Non-Maskable Interrupt request.
  void HandleNMI();
  // Perform one clock cycle's update.
  void HandleClocks();
  // Assisstive: Indicate the current instruction has completed by returning
  // true.
  bool HasCompleted();
  // Assisstive: Link this CPU to a communications bus.
  void ConnectTo(bus::Bus* bus) { bus_ = bus; }

 private:
  // CPU Instruction.
  struct Instruction {
    std::string name;
    types::uint8_t (CPU6502::*AddressingMode)(void) = nullptr;
    types::uint8_t (CPU6502::*Opcode)(void) = nullptr;
    types::uint8_t cycles = 0;
  };

  // Get Status Flag.
  types::uint8_t GetFlag(Status status);
  // Set Status Flag.
  void SetFlag(Status status, bool v);
  // Read content of the bus at the address.
  types::uint8_t Read(types::uint16_t address);
  // Write content to the bus at the address.
  void Write(types::uint16_t address, types::uint8_t v);
  // The read location of data can come from two sources, a memory address, or as part of the instruction.
  types::uint8_t Fetch();

  // Linkage to the communications bus.
  bus::Bus* bus_ = nullptr;
  // Assisstive: Lookup table of opcodes.
  std::vector<Instruction> lookup_;
  // Assisstive: Instruction byte.
  types::uint8_t opcode_ = 0x00;
  // Assisstive: Working input value to the ALU.
  types::uint8_t operand_ = 0x00;
  // Assisstive: Currently handling memory address.
  types::uint16_t address_ = 0x0000;
  // Assisstive: Used for relative addressing.
  types::uint16_t address_relative_ = 0x0000;
  // Assisstive: Count how many cycles the instruction has remaining.
  types::uint32_t cycles_ = 0;
  // Assisstive: A global accumulation of the number of clocks.
  types::uint32_t clock_count_ = 0;

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
};

}  // namespace cpu
}  // namespace nes
#endif
