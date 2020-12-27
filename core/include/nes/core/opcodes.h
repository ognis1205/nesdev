/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_OPCODES_H_
#define _NES_CORE_OPCODES_H_
#include <string>
#include "nes/core/types.h"

namespace nes {
namespace core {

/*
 * NOTE:
 * The following instructions are defined according to these references:
 * [SEE] https://www.masswerk.at/6502/6502_instruction_set.html
 * [SEE] https://undisbeliever.net/snesdev/65816-opcodes.htm
 * [SEE] http://nparker.llx.com/a2/opcodes.html
 */
enum AddressingMode : Byte {
  ABS,  // Absolute
  ABX,  // Absolute Indexed, X
  ABY,  // Absolute Indexed, Y
  ACC,  // Accumulator
  IMM,  // Immediate
  IMP,  // Implied
  IND,  // Indirect
  IZX,  // Indirect Indexed, X
  IZY,  // Indirect Indexed, Y
  REL,  // Relative
  ZP0,  // Zeropage
  ZPX,  // Zeropage X-Indexed
  ZPY,  // Zeropage Y-Indexed
  SR,   // ***65C816*** sr,S     / Stack Rerative
  DP,   // ***65C816*** dp       / Direct Page
  DPX,  // ***65C816*** dp,X     / Direct Page Indexed, X
  DPY,  // ***65C816*** dp,Y     / Direct Page Indexed, Y
  IDP,  // ***65C816*** (dp)     / Direct Page Indirect
  IDX,  // ***65C816*** (dp,X)   / Direct Page Indirect Indexed, X
  IDY,  // ***65C816*** (dp),Y   / Direct Page Indirect Indexed, Y
  IDL,  // ***65C816*** [dp]     / Direct Page Indirect Long
  IDLY, // ***65C816*** [dp],Y   / Direct Page Indirect Long Indexed, Y
  ISY,  // ***65C816*** (dp,S),Y / Stack Relative Indirect Indexed
  ABL,  // ***65C816*** al       / Absolute Long
  ALX,  // ***65C816*** al,X     / Absolute Long Indexed, X
  IAX,  // ***65C816*** (a)      / Absolute Indirect
  IAL,  // ***65C816*** (a,X)    / Absolute Indexed Indirect, X
  RELL, // ***65C816*** rl       / Relative Long
  BM    // ***65C816*** s,d      / Block Move
};

/*
 * NOTE:
 * The following instructions are defined according to these references:
 * [SEE] https://www.masswerk.at/6502/6502_instruction_set.html
 * [SEE] https://undisbeliever.net/snesdev/65816-opcodes.htm
 * [SEE] http://nparker.llx.com/a2/opcodes.html
 */
enum Instruction : Byte {
  ADC, // [Add]
  AND, // [And]
  ASL, // [ArithmeticShiftLeft]
  BCC, // [Branch] if Carry Clear
  BCS, // [Branch] if Carry Set
  BEQ, // [Branch] if Equal (z flag=1)
  BNE, // [Branch] if Not Equal (z=0)
  BMI, // [Branch] if Minus
  BPL, // [Branch] if Plus
  BVC, // [Branch] if Overflow Clear
  BVS, // [Branch] if Overflow Set
  BRA, // ***65C02-*** [Branch] Always
  BRL, // ***65C816*** [Branch] Always Long
  BIT, // [TestMemoryBits]
  BRK, // [SoftwareInterrupt] Break
  COP, // ***65C816*** [SoftwareInterrupt] Co-processor
  CLC, // [ClearStatusFlags] Clear Carry Flag
  CLI, // [ClearStatusFlags] Clear Interrupt Disable Flag
  CLD, // [ClearStatusFlags] Clear Decimal Flag
  CLV, // [ClearStatusFlags] Clear Overflow Flag
  CMP, // [Compare] Accumulator with Memory
  CPX, // [Compare] Index Register X with Memory
  CPY, // [Compare] Index Register Y with Memory
  DEC, // [Decrement]
  DEX, // [Decrement] Index Register X
  DEY, // [Decrement] Index Register Y
  EOR, // [ExclusiveOr]
  INC, // [Increment]
  INX, // [Increment] Index Register X
  INY, // [Increment] Index Register Y
  JMP, // [Jump]
  JML, // ***65C816*** [Jump]
  JSR, // [Jump] to Subroutine
  JSL, // ***65C816*** [Jump] to Subroutine
  LDA, // [Load] Accumulator from Memory
  LDX, // [Load] Index Register X from Memory
  LDY, // [Load] Index Register Y from Memory
  LSR, // [LogicalShiftRight]
  MVN, // ***65C816*** [BLockMove] Next
  MVP, // ***65C816*** [BLockMove] Previous
  NOP, // [NoOperation]
  ORA, // [Or] Accumulator with Memory
  PEA, // ***65C816*** [Push] Effective Absolute Address
  PEI, // ***65C816*** [Push] Effective Indirect Address
  PER, // ***65C816*** [Push] Effective PC Relative Indirect Address
  PHA, // [Push] Accumulator
  PHB, // ***65C816*** [Push] Data Bank
  PHD, // ***65C816*** [Push] Direct Page Register
  PHK, // ***65C816*** [Push] Program Bank Register
  PHP, // [Push] Processor Status Register
  PHX, // ***65C02-*** [Push] Index Register X
  PHY, // ***65C02-*** [Push] Index Register Y
  PLA, // [Pull] Accumulator
  PLB, // ***65C816*** [Pull] Data Bank
  PLD, // ***65C816*** [Pull] Direct Page Register
  PLP, // [Pull] Processor Status Register
  PLX, // ***65C02-*** [Pull] Index Register X
  PLY, // ***65C02-*** [Pull] Index Register Y
  REP, // ***65C816*** [ResetStatusBits]
  ROL, // [Rotate] Left
  ROR, // [Rotate] Right
  RTI, // [Return] From Interrupt
  RTS, // [Return] From Subroutine
  RTL, // ***65C816*** [Return] From Subroutine (return long)
  SBC, // [Subtract] with Borrow from Accumulator
  SEC, // [Set] Carry Flag
  SEI, // [Set] Interrupt Disable Flag
  SED, // [Set] Decimal Flag
  SEP, // [Set] Status Bits
  STP, // ***65C816*** [Stop] the Processor
  STA, // [Store] Accumulator to Memory
  STX, // [Store] Index Register X to Memory
  STY, // [Store] Index Register Y to Memory
  STZ, // ***65C02-*** [Store] Zero to Memory
  TAX, // [Transfer] A to X
  TAY, // [Transfer] A to Y
  TCD, // ***65C816*** [Transfer] 16 bit A to DP
  TCS, // ***65C816*** [Transfer] 16 bit A to SP
  TDC, // ***65C816*** [Transfer] DP to 16 bit A
  TSC, // ***65C816*** [Transfer] SP to 16 bit A
  TSX, // [Transfer] SP to X
  TXA, // [Transfer] X to A
  TXS, // [Transfer] X to SP
  TXY, // ***65C816*** [Transfer] X to Y
  TYA, // [Transfer] Y to A
  TYX, // ***65C816*** [Transfer] Y to X
  TRB, // ***65C02-*** [Test] and Reset Memory Bits Against Accumulator
  TSB, // ***65C02-*** [Test] and Set Memory Bits Against Accumulator
  WAI, // ***65C816*** [Wait] for Interrupt
  WDM, // ***65C816*** [Reserved] for Future Expansion
  XBA, // ***65C816*** [Exchange] the B and A Accumulators
  XCE  // ***65C816*** [Exchange] Carry and Emulation Bits
};

enum class MemoryAccess {
  READ,
  READ_MODIFY_WRITE,
  WRITE
};

struct Opcode {
  Instruction instruction;
  AddressingMode addressing_mode;
  MemoryAccess memory_access;
};

[[nodiscard]]
Opcode Decode(const Byte& opcode) noexcept;

[[nodiscard]]
std::string ToString(const Byte& opcode) noexcept;

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_OPCODES_H_
