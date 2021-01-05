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

#define REGISTER(x)    registers_->x.value
#define REGISTER_LO(x) registers_->x.lo
#define REGISTER_HI(x) registers_->x.hi
#define FLAG(x)        registers_->p.x
#define MASK(x)        registers_->p.x.mask

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
    FetchOpcode();
//    }
  } else {
    Execute();
  }
  //++state_.cycle;
}

/*
 * [SEE] https://robinli.eu/f/6502_cpu.txt
 */
void MOS6502::FetchOpcode() {
  // Parse next instruction.
  ReadOpcode();

  // The following addressing modes are not supported on MOS6502 archetecture.
  switch (GetAddressingMode()) {
  case AddressingMode::SR  : case AddressingMode::DP  : case AddressingMode::DPX : case AddressingMode::DPY :
  case AddressingMode::IDP : case AddressingMode::IDX : case AddressingMode::IDY : case AddressingMode::IDL :
  case AddressingMode::IDLY: case AddressingMode::ISY : case AddressingMode::ABL : case AddressingMode::ALX :
  case AddressingMode::IAX : case AddressingMode::IAL : case AddressingMode::RELL: case AddressingMode::BM  :
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid addressing mode specified to Fetch", GetOpcode()));
  default:
    break;
  }

  // The following instructions are not supported on MOS6502 archetecture.
  switch (GetInstruction()) {
  case Instruction::BRA: case Instruction::BRL: case Instruction::COP: case Instruction::JML: case Instruction::JSL:
  case Instruction::MVN: case Instruction::MVP: case Instruction::PEA: case Instruction::PEI: case Instruction::PER:
  case Instruction::PHB: case Instruction::PHD: case Instruction::PHK: case Instruction::PHX: case Instruction::PHY:
  case Instruction::PLB: case Instruction::PLD: case Instruction::PLX: case Instruction::PLY: case Instruction::REP:
  case Instruction::RTL: case Instruction::STP: case Instruction::STZ: case Instruction::TCD: case Instruction::TCS:
  case Instruction::TDC: case Instruction::TSC: case Instruction::TXY: case Instruction::TYX: case Instruction::TRB:
  case Instruction::TSB: case Instruction::WAI: case Instruction::WDM: case Instruction::XBA: case Instruction::XCE:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  default:
    break;
  }

  // Stage instructions in accordance with the specified addressing mode.
  switch (GetAddressingMode()) {
  case AddressingMode::ACC: ACC(); return;
  case AddressingMode::IMP: IMP(); return;
  case AddressingMode::IMM: IMM(); return;
  case AddressingMode::ABS: ABS(); return;
  default:
    break;
  }
}

void MOS6502::ACC() {
  switch (GetInstruction()) {
  case Instruction::ASL: Stage([this] { REGISTER(a) = ShiftL(REGISTER(a), false); }); break;
  case Instruction::ROL: Stage([this] { REGISTER(a) = ShiftL(REGISTER(a), true);  }); break;
  case Instruction::LSR: Stage([this] { REGISTER(a) = ShiftR(REGISTER(a), false); }); break;
  case Instruction::ROR: Stage([this] { REGISTER(a) = ShiftR(REGISTER(a), true);  }); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::IMP() {
  switch (GetInstruction()) {
  case Instruction::BRK:
    Stage([this] { REGISTER(p) |= MASK(irq_disable);
                   Read(REGISTER(pc)++); });
    Stage([this] { Push(REGISTER_HI(pc)); });
    Stage([this] { Push(REGISTER_LO(pc)); });
    Stage([this] { Push(REGISTER(p) | MASK(brk_command)); });
    Stage([this] { REGISTER_LO(pc) = Read(MOS6502::kBRKAddress); });
    Stage([this] { REGISTER_HI(pc) = Read(MOS6502::kBRKAddress + 1); });
    break;
  case Instruction::PHP:
    Stage([this] { Read(REGISTER(pc)); });
    Stage([this] { Push(REGISTER(p) | MASK(brk_command) | MASK(unused));
                   REGISTER(p) &= ~(MASK(brk_command) | MASK(unused)); });
    break;
  case Instruction::CLC:
    Stage([this] { REGISTER(p) &= ~MASK(carry); });
    break;
  case Instruction::PLP:
    Stage([this] { Read(REGISTER(pc)); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REGISTER(p) = Pull();
                   REGISTER(p) |= MASK(unused); });
    break;
  case Instruction::SEC:
    Stage([this] { REGISTER(p) |= MASK(carry); });
    break;
  case Instruction::RTI:
    Stage([this] { Read(REGISTER(pc)); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REGISTER(p) = Pull();
                   REGISTER(p) &= ~(MASK(brk_command) | MASK(unused)); });
    Stage([this] { REGISTER_LO(pc) = Pull(); });
    Stage([this] { REGISTER_HI(pc) = Pull(); });
    break;
  case Instruction::PHA:
    Stage([this] { Read(REGISTER(pc)); });
    Stage([this] { Push(REGISTER(a)); });
    break;
  case Instruction::CLI:
    Stage([this] { REGISTER(p) &= ~MASK(irq_disable); });
    break;
  case Instruction::RTS:
    Stage([this] { Read(REGISTER(pc)); });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REGISTER_LO(pc) = Pull(); });
    Stage([this] { REGISTER_HI(pc) = Pull(); });
    Stage([this] { REGISTER(pc)++; });
    break;
  case Instruction::PLA:
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REGISTER(a) = PassThrough(Pull()); });
    break;
  case Instruction::SEI: Stage([this] { REGISTER(p) |= MASK(irq_disable);        }); break;
  case Instruction::DEY: Stage([this] { REGISTER(y)  = Decrement(REGISTER(y));   }); break;
  case Instruction::TXA: Stage([this] { REGISTER(a)  = PassThrough(REGISTER(x)); }); break;
  case Instruction::TYA: Stage([this] { REGISTER(a)  = PassThrough(REGISTER(y)); }); break;
  case Instruction::TXS: Stage([this] { REGISTER(s)  = REGISTER(x);              }); break;
  case Instruction::TAY: Stage([this] { REGISTER(y)  = PassThrough(REGISTER(a)); }); break;
  case Instruction::TAX: Stage([this] { REGISTER(x)  = PassThrough(REGISTER(a)); }); break;
  case Instruction::CLV: Stage([this] { REGISTER(p) &= ~MASK(overflow);          }); break;
  case Instruction::TSX: Stage([this] { REGISTER(x)  = PassThrough(REGISTER(s)); }); break;
  case Instruction::INY: Stage([this] { REGISTER(y)  = Increment(REGISTER(y));   }); break;
  case Instruction::DEX: Stage([this] { REGISTER(x)  = Decrement(REGISTER(x));   }); break;
  case Instruction::CLD: Stage([this] { REGISTER(p) &= ~MASK(decimal_mode);      }); break;
  case Instruction::INX: Stage([this] { REGISTER(x)  = Increment(REGISTER(x));   }); break;
  case Instruction::NOP: Stage([    ] { /* Do nothing. */                        }); break;
  case Instruction::SED: Stage([this] { REGISTER(p) |= MASK(decimal_mode);       }); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::IMM() {
  switch (GetInstruction()) {
  case Instruction::ORA: Stage([this] { REGISTER(a) =  Or(REGISTER(a), Fetch()); }); break;
  case Instruction::AND: Stage([this] { REGISTER(a) = And(REGISTER(a), Fetch()); }); break;
  case Instruction::EOR: Stage([this] { REGISTER(a) = Xor(REGISTER(a), Fetch()); }); break;
  case Instruction::ADC: Stage([this] { REGISTER(a) = Add(REGISTER(a), Fetch()); }); break;
  case Instruction::LDY: Stage([this] { REGISTER(y) = PassThrough(Fetch());      }); break;
  case Instruction::LDX: Stage([this] { REGISTER(x) = PassThrough(Fetch());      }); break;
  case Instruction::LDA: Stage([this] { REGISTER(a) = PassThrough(Fetch());      }); break;
  case Instruction::CPY: Stage([this] { Cmp(REGISTER(y), Fetch());               }); break;
  case Instruction::CMP: Stage([this] { Cmp(REGISTER(a), Fetch());               }); break;
  case Instruction::CPX: Stage([this] { Cmp(REGISTER(x), Fetch());               }); break;
  case Instruction::SBC: Stage([this] { REGISTER(a) = Sub(REGISTER(a), Fetch()); }); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::ABS() {
  if (Is(Instruction::JSR)) {
    Stage([this] { SetAddressLo(Read(REGISTER(pc)++)); });
    Stage([    ] { /* Internal operation (predecrement S?). */ });
    Stage([this] { Push(REGISTER_HI(pc)); });
    Stage([this] { Push(REGISTER_LO(pc)); });
    Stage([this] { SetAddressHi(Read(REGISTER(pc)++));
                   REGISTER(pc) = GetAddress(); });
    return;
  }

  if (Is(Instruction::JMP)) {
    Stage([this] { SetAddressLo(Read(REGISTER(pc)++)); });
    Stage([this] { SetAddressHi(Read(REGISTER(pc)++));
                   REGISTER(pc) = GetAddress(); });
    return;
  }

  Stage([this] { SetAddressLo(Read(REGISTER(pc)++)); });
  Stage([this] { SetAddressHi(Read(REGISTER(pc)++)); });
  if (Is(MemoryAccess::READ_MODIFY_WRITE)) {
    Stage([this] { Fetch(); });
    Stage([this] { Write(GetAddress(), Fetched()); });
  }

  switch (GetInstruction()) {
  case Instruction::ORA: Stage([this] { REGISTER(a) = Or(REGISTER(a), Fetch());        }); break;
  case Instruction::ASL: Stage([this] { Write(GetAddress(), ShiftL(Fetched(), false)); }); break;
  case Instruction::JSR: /* JSR is handled above. */                                       break;
  case Instruction::BIT: Stage([this] { Bit(REGISTER(a), Fetch());                     }); break;
  case Instruction::AND: Stage([this] { REGISTER(a) = And(REGISTER(a), Fetch());       }); break;
  case Instruction::ROL: Stage([this] { Write(GetAddress(), ShiftL(Fetched(), true));  }); break;
  case Instruction::JMP: /* JMP is handled above. */                                       break;
  case Instruction::EOR: Stage([this] { REGISTER(a) = Xor(REGISTER(a), Fetch());       }); break;
  case Instruction::LSR: Stage([this] { Write(GetAddress(), ShiftR(Fetched(), false)); }); break;
  case Instruction::ADC: Stage([this] { REGISTER(a) = Add(REGISTER(a), Fetch());       }); break;
  case Instruction::ROR: Stage([this] { Write(GetAddress(), ShiftR(Fetched(), true));  }); break;
  case Instruction::STY: Stage([this] { Write(GetAddress(), REGISTER(y));              }); break;
  case Instruction::STA: Stage([this] { Write(GetAddress(), REGISTER(a));              }); break;
  case Instruction::STX: Stage([this] { Write(GetAddress(), REGISTER(x));              }); break;
  case Instruction::LDY: Stage([this] { REGISTER(y) = PassThrough(Fetch());            }); break;
  case Instruction::LDA: Stage([this] { REGISTER(a) = PassThrough(Fetch());            }); break;
  case Instruction::LDX: Stage([this] { REGISTER(x) = PassThrough(Fetch());            }); break;
  case Instruction::CPY: Stage([this] { Cmp(REGISTER(y), Fetch());                     }); break;
  case Instruction::CMP: Stage([this] { Cmp(REGISTER(a), Fetch());                     }); break;
  case Instruction::DEC: Stage([this] { Write(GetAddress(), Decrement(Fetched()));     }); break;
  case Instruction::CPX: Stage([this] { Cmp(REGISTER(x), Fetch());                     }); break;
  case Instruction::SBC: Stage([this] { REGISTER(a) = Sub(REGISTER(a), Fetch());       }); break;
  case Instruction::INC: Stage([this] { Write(GetAddress(), Increment(Fetched()));     }); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::RST() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

