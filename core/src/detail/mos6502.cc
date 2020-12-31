/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "nesdev/core/cpu.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"
#include "macros.h"
#include "pipeline.h"
#include "detail/mos6502.h"

namespace nesdev {
namespace core {
namespace detail {

MOS6502::Stack::Stack(Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu} {}

Byte MOS6502::Stack::Pull() const {
  return mmu_->Read(Stack::kOffset + ++registers_->s.value);
}

void MOS6502::Stack::Push(Byte byte) {
  mmu_->Write(Stack::kOffset + registers_->s.value--, byte);
}

MOS6502::MOS6502(MOS6502::Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu}, stack_{registers, mmu} {}

MOS6502::~MOS6502() {}

void MOS6502::Tick() {
  if (ClearWhenCompletion()) {
//    if (nmi_) {
//      pipeline_ = create_nmi();
//      nmi_ = false;
//    } else {
      //TODO: Clear all context? pipeline...
    Fetch();
//    }
  } else {
    Execute();
  }
  //++state_.cycle;
}

/*
 * SEE: https://robinli.eu/f/6502_cpu.txt
 */
void MOS6502::Fetch() {
  auto opcode = Read(registers_->pc.value++);
  context_.opcode = Decode(opcode);

  // The following addressing modes are not supported on MOS6502 archetecture.
  switch (context_.opcode->addressing_mode) {
  case AddressingMode::SR  : case AddressingMode::DP  : case AddressingMode::DPX : case AddressingMode::DPY :
  case AddressingMode::IDP : case AddressingMode::IDX : case AddressingMode::IDY : case AddressingMode::IDL :
  case AddressingMode::IDLY: case AddressingMode::ISY : case AddressingMode::ABL : case AddressingMode::ALX :
  case AddressingMode::IAX : case AddressingMode::IAL : case AddressingMode::RELL: case AddressingMode::BM  :
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid addressing mode specified to Fetch", opcode));
  default:
    break;
  }

  // The following instructions are not supported on MOS6502 archetecture.
  switch (context_.opcode->instruction) {
  case Instruction::BRA: case Instruction::BRL: case Instruction::COP: case Instruction::JML: case Instruction::JSL:
  case Instruction::MVN: case Instruction::MVP: case Instruction::PEA: case Instruction::PEI: case Instruction::PER:
  case Instruction::PHB: case Instruction::PHD: case Instruction::PHK: case Instruction::PHX: case Instruction::PHY:
  case Instruction::PLB: case Instruction::PLD: case Instruction::PLX: case Instruction::PLY: case Instruction::REP:
  case Instruction::RTL: case Instruction::STP: case Instruction::STZ: case Instruction::TCD: case Instruction::TCS:
  case Instruction::TDC: case Instruction::TSC: case Instruction::TXY: case Instruction::TYX: case Instruction::TRB:
  case Instruction::TSB: case Instruction::WAI: case Instruction::WDM: case Instruction::XBA: case Instruction::XCE:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  default:
    break;
  }

  // Handle instructions addressing stack.
  switch (context_.opcode->instruction) {
  case Instruction::BRK:
    Stage([this] { registers_->p.value |= registers_->p.irq_disable.mask;
                   Read(registers_->pc.value++); });
    Stage([this] { Push(registers_->pc.hi); });
    Stage([this] { Push(registers_->pc.lo); });
    Stage([this] { Push(registers_->p.value | registers_->p.brk_command.mask); });
    Stage([this] { registers_->pc.lo = Read(MOS6502::kBRKAddress); });
    Stage([this] { registers_->pc.hi = Read(MOS6502::kBRKAddress + 1); });
    return;

  case Instruction::RTI:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->p.value = Pull();
                   registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    Stage([this] { registers_->pc.lo = Pull(); });
    Stage([this] { registers_->pc.hi = Pull(); });
    return;

  case Instruction::RTS:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->pc.lo = Pull(); });
    Stage([this] { registers_->pc.hi = Pull(); });
    Stage([this] { registers_->pc.value++; });
    return;

  case Instruction::PHA:
    Stage([this] { Read(registers_->pc.value); });
    Stage([this] { Push(registers_->a.value); });
    return;

  case Instruction::PHP:
    Stage([this] { Read(registers_->pc.value); });
    Stage([this] { Push(registers_->p.value | registers_->p.brk_command.mask | registers_->p.unused.mask);
                   registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    return;

  case Instruction::PLA:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->a.value = Pull();
                   if (registers_->a.value == 0x00) registers_->p.value |= registers_->p.zero.mask;
                   if (registers_->a.value & 0x80)  registers_->p.value |= registers_->p.negative.mask; });
    return;

  case Instruction::PLP:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->p.value = Pull();
                   registers_->p.value |= registers_->p.unused.mask; });
    return;

  case Instruction::JSR:
    Stage([this] { context_.effective_address.lo = Read(registers_->pc.value++); });
    Stage([    ] { /* Internal operation (predecrement S?). */ });
    Stage([this] { Push(registers_->pc.hi); });
    Stage([this] { Push(registers_->pc.lo); });
    Stage([this] { context_.effective_address.hi = Read(registers_->pc.value);
                   registers_->pc.value = context_.effective_address.value; });
    return;

  default:
    break;
  }

//  switch (context_.opcode->addressing_mode) {
//  case AddressingMode::ACC:
//  case AddressingMode::IMP:
//    Stage([this] { Read(registers_->pc.value); });
//    break;
//  case AddressingMode::IMM:
//    Stage([this] { context_.effective_address.value = registers_->pc.value++; });
//    break;
//  case AddressingMode::ABS:
//    Stage([this] { context_.effective_address.lo = registers_->pc.value++; });
//    Stage([this] { context_.effective_address.hi = registers_->pc.value++; });
//    break;
//  default:
//    break;
//  }
}

void MOS6502::Reset() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

