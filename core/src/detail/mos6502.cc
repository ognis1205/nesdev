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

MOS6502::ALU::ALU(Registers* const registers)
  : registers_{registers} {}

Byte MOS6502::ALU::ShiftL(MOS6502::ALU::Bus bus, bool rotate_carry) {
  bus.concat <<= 1;
  bus.b |= rotate_carry ? registers_->p.carry : 0x00;
  registers_->p.carry    = bus.a & 0x01;
  registers_->p.zero     = bus.b == 0x00;
  registers_->p.negative = bus.b & 0x80;
  return bus.b;
}

Byte MOS6502::ALU::ShiftR(MOS6502::ALU::Bus bus, bool rotate_carry) {
  bus.concat >>= 1;
  bus.a |= rotate_carry ? registers_->p.carry << 7 : 0x00;
  registers_->p.carry    = bus.b & 0x80;
  registers_->p.zero     = bus.a == 0x00;
  registers_->p.negative = bus.a & 0x80;
  return bus.a;
}

MOS6502::MOS6502(MOS6502::Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu}, stack_{registers, mmu}, alu_{registers} {}

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
    Stage(&MOS6502::ACC, context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  case AddressingMode::IMP:
    Stage(&MOS6502::IMP, context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  case AddressingMode::IMM:
    Stage(&MOS6502::IMM, context_.opcode->instruction, context_.opcode->memory_access, opcode);
    return;
  default:
    break;
  }
}

Pipeline MOS6502::ACC(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  Pipeline pipeline;

  switch (instruction) {
  case Instruction::ASL:
    pipeline.Push([this] { registers_->a.value = ShiftL(registers_->a.value, false); });
    break;
  case Instruction::ROL:
    pipeline.Push([this] { registers_->a.value = ShiftL(registers_->a.value, true); });
    break;
  case Instruction::LSR:
    pipeline.Push([this] { registers_->a.value = ShiftR(registers_->a.value, false); });
    break;
  case Instruction::ROR:
    pipeline.Push([this] { registers_->a.value = ShiftR(registers_->a.value, true); });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }

  return pipeline;
}

Pipeline MOS6502::IMP(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  Pipeline pipeline;

  switch (instruction) {
  case Instruction::BRK:
    pipeline.Push([this] { registers_->p.value |= registers_->p.irq_disable.mask; Read(registers_->pc.value++); });
    pipeline.Push([this] { Push(registers_->pc.hi); });
    pipeline.Push([this] { Push(registers_->pc.lo); });
    pipeline.Push([this] { Push(registers_->p.value | registers_->p.brk_command.mask); });
    pipeline.Push([this] { registers_->pc.lo = Read(MOS6502::kBRKAddress); });
    pipeline.Push([this] { registers_->pc.hi = Read(MOS6502::kBRKAddress + 1); });
    break;
  case Instruction::PHP:
    pipeline.Push([this] { Read(registers_->pc.value); });
    pipeline.Push([this] { Push(registers_->p.value | registers_->p.brk_command.mask | registers_->p.unused.mask);
                           registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    break;
  case Instruction::CLC:
    pipeline.Push([this] { registers_->p.value &= ~registers_->p.carry.mask; });
    break;
  case Instruction::PLP:
    pipeline.Push([this] { Read(registers_->pc.value); });
    pipeline.Push([    ] { /* Increment stack pointer. Done in Pull. */ });
    pipeline.Push([this] { registers_->p.value = Pull();
                           registers_->p.value |= registers_->p.unused.mask; });
    break;
  case Instruction::SEC:
    pipeline.Push([this] { registers_->p.value |= registers_->p.carry.mask; });
    break;
  case Instruction::RTI:
    pipeline.Push([this] { Read(registers_->pc.value); });
    pipeline.Push([    ] { /* Increment stack pointer. Done in Pull. */ });
    pipeline.Push([this] { registers_->p.value = Pull();
                           registers_->p.value &= ~(registers_->p.brk_command.mask | registers_->p.unused.mask); });
    pipeline.Push([this] { registers_->pc.lo = Pull(); });
    pipeline.Push([this] { registers_->pc.hi = Pull(); });
    break;
  case Instruction::PHA:
    pipeline.Push([this] { Read(registers_->pc.value); });
    pipeline.Push([this] { Push(registers_->a.value); });
    break;
  case Instruction::CLI:
    pipeline.Push([this] { registers_->p.value &= ~registers_->p.irq_disable.mask; });
    break;
  case Instruction::RTS:
    pipeline.Push([this] { Read(registers_->pc.value); });
    pipeline.Push([    ] { /* Increment stack pointer. Done in Pull. */ });
    pipeline.Push([this] { registers_->pc.lo = Pull(); });
    pipeline.Push([this] { registers_->pc.hi = Pull(); });
    pipeline.Push([this] { registers_->pc.value++; });
    break;
  case Instruction::PLA:
    pipeline.Push([    ] { /* Increment stack pointer. Done in Pull. */ });
    pipeline.Push([this] { registers_->a.value = Pull();
                           registers_->p.zero     = registers_->a.value == 0x00;
                           registers_->p.negative = registers_->a.value & 0x80; });
    break;
  case Instruction::SEI:
    pipeline.Push([this] { registers_->p.value |= registers_->p.irq_disable.mask; });
    break;
  case Instruction::DEY:
    pipeline.Push([this] { --registers_->y.value;
                           registers_->p.zero     = registers_->y.value == 0x00;
                           registers_->p.negative = registers_->y.value & 0x80; });
    break;
  case Instruction::TXA:
    pipeline.Push([this] { registers_->a.value = registers_->x.value;
                           registers_->p.zero     = registers_->a.value == 0x00;
                           registers_->p.negative = registers_->a.value & 0x80; });
    break;
  case Instruction::TYA:
    pipeline.Push([this] { registers_->a.value = registers_->y.value;
                           registers_->p.zero     = registers_->a.value == 0x00;
                           registers_->p.negative = registers_->a.value & 0x80; });
    break;
  case Instruction::TXS:
    pipeline.Push([this] { registers_->s.value = registers_->x.value; });
    break;
  case Instruction::TAY:
    pipeline.Push([this] { registers_->y.value = registers_->a.value;
                           registers_->p.zero     = registers_->y.value == 0x00;
                           registers_->p.negative = registers_->y.value & 0x80; });
    break;
  case Instruction::TAX:
    pipeline.Push([this] { registers_->x.value = registers_->a.value;
                           registers_->p.zero     = registers_->x.value == 0x00;
                           registers_->p.negative = registers_->x.value & 0x80; });
    break;
  case Instruction::CLV:
    pipeline.Push([this] { registers_->p.value &= ~registers_->p.overflow.mask; });
    break;
  case Instruction::TSX:
    pipeline.Push([this] { registers_->x.value = registers_->s.value;
                           registers_->p.zero     = registers_->x.value == 0x00;
                           registers_->p.negative = registers_->x.value & 0x80; });
    break;
  case Instruction::INY:
    pipeline.Push([this] { ++registers_->y.value;
                           registers_->p.zero     = registers_->y.value == 0x00;
                           registers_->p.negative = registers_->y.value & 0x80; });
    break;
  case Instruction::DEX:
    pipeline.Push([this] { --registers_->x.value;
                           registers_->p.zero     = registers_->x.value == 0x00;
                           registers_->p.negative = registers_->x.value & 0x80; });
    break;
  case Instruction::CLD:
    pipeline.Push([this] { registers_->p.value &= ~registers_->p.decimal_mode.mask; });
    break;
  case Instruction::INX:
    pipeline.Push([this] { ++registers_->x.value;
                           registers_->p.zero     = registers_->x.value == 0x00;
                           registers_->p.negative = registers_->x.value & 0x80; });
    break;
  case Instruction::NOP:
    Stage([    ] { /* Do nothing. */ });
    break;
  case Instruction::SED:
    pipeline.Push([this] { registers_->p.value |= registers_->p.decimal_mode.mask; });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }

  return pipeline;
}

Pipeline MOS6502::IMM(Instruction instruction, [[maybe_unused]]MemoryAccess memory_access, const Byte& opcode) {
  Pipeline pipeline;

  switch (instruction) {
  case Instruction::ASL:
    pipeline.Push([this] { registers_->a.value = ShiftL(registers_->a.value, false); });
    break;
  case Instruction::ROL:
    pipeline.Push([this] { registers_->a.value = ShiftL(registers_->a.value, true); });
    break;
  case Instruction::LSR:
    pipeline.Push([this] { registers_->a.value = ShiftR(registers_->a.value, false); });
    break;
  case Instruction::ROR:
    pipeline.Push([this] { registers_->a.value = ShiftR(registers_->a.value, true); });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", opcode));
  }

  return pipeline;
}

void MOS6502::Reset() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

