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

MOS6502::MOS6502(MOS6502::Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu}, stack_{registers, mmu}, alu_{registers} {}

MOS6502::~MOS6502() {}

void MOS6502::Tick() {
  if (ClearWhenCompleted()) {
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
  case Instruction::JSR:
    Stage([this] { context_.address.lo = Read(registers_->pc.value++); });
    Stage([    ] { /* Internal operation (predecrement S?). */ });
    Stage([this] { Push(registers_->pc.hi); });
    Stage([this] { Push(registers_->pc.lo); });
    Stage([this] { context_.address.hi = Read(registers_->pc.value);
                   registers_->pc.value = context_.address.effective; });
    return;
  default:
    break;
  }

  // Handle Accumulator or implied addressing mode.
  switch (context_.opcode->addressing_mode) {
  case AddressingMode::ACC:
    WithACC(context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  case AddressingMode::IMP:
    WithIMP(context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  case AddressingMode::IMM:
    WithIMM(context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  default:
    break;
  }
}

void MOS6502::WithACC(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  switch (instruction) {
  case Instruction::ASL:
    Stage([this] { registers_->a.value = ShiftL(registers_->a.value, false); });
    break;
  case Instruction::ROL:
    Stage([this] { registers_->a.value = ShiftL(registers_->a.value, true); });
    break;
  case Instruction::LSR:
    Stage([this] { registers_->a.value = ShiftR(registers_->a.value, false); });
    break;
  case Instruction::ROR:
    Stage([this] { registers_->a.value = ShiftR(registers_->a.value, true); });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }
}

void MOS6502::WithIMP(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  switch (instruction) {
  case Instruction::BRK:
    Stage([this] { registers_->p.value |= registers_->p.irq_disable.mask;
	           Read(registers_->pc.value++); });
    Stage([this] { Push(registers_->pc.hi); });
    Stage([this] { Push(registers_->pc.lo); });
    Stage([this] { Push(registers_->p.value | registers_->p.brk_command.mask); });
    Stage([this] { registers_->pc.lo = PassThrough(Read(MOS6502::kBRKAddress), false); });
    Stage([this] { registers_->pc.hi = PassThrough(Read(MOS6502::kBRKAddress + 1), false); });
    break;
  case Instruction::PHP:
    Stage([this] { Read(registers_->pc.value); });
    Stage([this] { Push(registers_->p.value | registers_->p.brk_command.mask | registers_->p.unused.mask);
	           registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    break;
  case Instruction::CLC:
    Stage([this] { registers_->p.value &= ~registers_->p.carry.mask; });
    break;
  case Instruction::PLP:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->p.value = PassThrough(Pull(), false);
	           registers_->p.value |= registers_->p.unused.mask; });
    break;
  case Instruction::SEC:
    Stage([this] { registers_->p.value |= registers_->p.carry.mask; });
    break;
  case Instruction::RTI:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->p.value = PassThrough(Pull(), false);
	           registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    Stage([this] { registers_->pc.lo = PassThrough(Pull(), false); });
    Stage([this] { registers_->pc.hi = PassThrough(Pull(), false); });
    break;
  case Instruction::PHA:
    Stage([this] { Read(registers_->pc.value); });
    Stage([this] { Push(registers_->a.value); });
    break;
  case Instruction::CLI:
    Stage([this] { registers_->p.value &= ~registers_->p.irq_disable.mask; });
    break;
  case Instruction::RTS:
    Stage([this] { Read(registers_->pc.value); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->pc.lo = PassThrough(Pull(), false); });
    Stage([this] { registers_->pc.hi = PassThrough(Pull(), false); });
    Stage([this] { registers_->pc.value++; });
    break;
  case Instruction::PLA:
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { registers_->a.value = PassThrough(Pull(), true); });
    break;
  case Instruction::SEI:
    Stage([this] { registers_->p.value |= registers_->p.irq_disable.mask; });
    break;
  case Instruction::DEY:
    Stage([this] { registers_->y.value = Decrement(registers_->y.value); });
    break;
  case Instruction::TXA:
    Stage([this] { registers_->a.value = PassThrough(registers_->x.value, true); });
    break;
  case Instruction::TYA:
    Stage([this] { registers_->a.value = PassThrough(registers_->y.value, true); });
    break;
  case Instruction::TXS:
    Stage([this] { registers_->s.value = PassThrough(registers_->x.value, false); });
    break;
  case Instruction::TAY:
    Stage([this] { registers_->y.value = PassThrough(registers_->a.value, true); });
    break;
  case Instruction::TAX:
    Stage([this] { registers_->x.value = PassThrough(registers_->a.value, true); });
    break;
  case Instruction::CLV:
    Stage([this] { registers_->p.value &= ~registers_->p.overflow.mask; });
    break;
  case Instruction::TSX:
    Stage([this] { registers_->x.value = PassThrough(registers_->s.value, true); });
    break;
  case Instruction::INY:
    Stage([this] { registers_->y.value = Increment(registers_->y.value); });
    break;
  case Instruction::DEX:
    Stage([this] { registers_->x.value = Decrement(registers_->x.value); });
    break;
  case Instruction::CLD:
    Stage([this] { registers_->p.value &= ~registers_->p.decimal_mode.mask; });
    break;
  case Instruction::INX:
    Stage([this] { registers_->x.value = Increment(registers_->x.value); });
    break;
  case Instruction::NOP:
    Stage([    ] { /* Do nothing. */ });
    break;
  case Instruction::SED:
    Stage([this] { registers_->p.value |= registers_->p.decimal_mode.mask; });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }
}

void MOS6502::WithIMM(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  context_.address.effective = registers_->pc.value++;
  switch (instruction) {
  case Instruction::ORA:
    Stage([this] { registers_->a.value = Or(registers_->a.value, Read(context_.address.effective)); });
    break;
  case Instruction::AND:
    Stage([this] { registers_->a.value = And(registers_->a.value, Read(context_.address.effective)); });
    break;
  case Instruction::EOR:
    Stage([this] { registers_->a.value = Xor(registers_->a.value, Read(context_.address.effective)); });
    break;
  case Instruction::ADC:
    Stage([this] { registers_->a.value = Add(registers_->a.value, Read(context_.address.effective)); });
    break;
  case Instruction::LDY:
    Stage([this] { registers_->y.value = PassThrough(Read(context_.address.effective), true); });
    break;
  case Instruction::LDX:
    Stage([this] { registers_->x.value = PassThrough(Read(context_.address.effective), true); });
    break;
  case Instruction::LDA:
    Stage([this] { registers_->a.value = PassThrough(Read(context_.address.effective), true); });
    break;
  case Instruction::CPY:
    Stage([this] { Cmp(registers_->y.value, Read(context_.address.effective)); });
    break;
  case Instruction::CMP:
    Stage([this] { Cmp(registers_->a.value, Read(context_.address.effective)); });
    break;
  case Instruction::CPX:
    Stage([this] { Cmp(registers_->x.value, Read(context_.address.effective)); });
    break;
  case Instruction::SBC:
    Stage([this] { registers_->a.value = Sub(registers_->a.value, Read(context_.address.effective)); });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }
}

void MOS6502::Reset() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

