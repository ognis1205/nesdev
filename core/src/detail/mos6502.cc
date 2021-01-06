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

#define REG(x)    registers_->x.value
#define REG_LO(x) registers_->x.lo
#define REG_HI(x) registers_->x.hi
#define FLG(x)    registers_->p.x
#define MSK(x)    registers_->p.x.mask
using A = AddressingMode;
using I = Instruction;
using M = MemoryAccess;
using S = Pipeline::Status;

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
 * The high byte of the effective address may be invalid in indexed addressing
 * modes. Because the processor cannot undo a write to an invalid address, it
 * always reads from the address first.
 */
inline Address FixHiByte(Address address) noexcept {
  return address - static_cast<Address>(0x0100);
}

/*
 * The following instruction timings are defined according to the following article.
 * [SEE] https://robinli.eu/f/6502_cpu.txt
 */
void MOS6502::FetchOpcode() {
  // Parse next instruction.
  ReadOpcode();

  // The following addressing modes are not supported on MOS6502 archetecture.
  switch (GetAddressingMode()) {
  case A::SR  : case A::DP  : case A::DPX : case A::DPY :
  case A::IDP : case A::IDX : case A::IDY : case A::IDL :
  case A::IDLY: case A::ISY : case A::ABL : case A::ALX :
  case A::IAX : case A::IAL : case A::RELL: case A::BM  :
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid addressing mode specified to Fetch", GetOpcode()));
  default:
    break;
  }

  // The following instructions are not supported on MOS6502 archetecture.
  switch (GetInstruction()) {
  case I::BRA: case I::BRL: case I::COP: case I::JML: case I::JSL: case I::MVN:
  case I::MVP: case I::PEA: case I::PEI: case I::PER: case I::PHB: case I::PHD:
  case I::PHK: case I::PHX: case I::PHY: case I::PLB: case I::PLD: case I::PLX:
  case I::PLY: case I::REP: case I::RTL: case I::SEP: case I::STP: case I::STZ:
  case I::TCD: case I::TCS: case I::TDC: case I::TSC: case I::TXY: case I::TYX:
  case I::TRB: case I::TSB: case I::WAI: case I::WDM: case I::XBA: case I::XCE:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  default:
    break;
  }

  // The following instructions (Jumps) are treated as a kind of special form of addressing in this implementation.
  if (Is(I::JMP) || Is(I::JSR)) {
    Stage([this] { SetAddressLo(Read(REG(pc)++));                         }, Is(I::JMP));
    Stage([this] { SetAddressHi(Read(REG(pc)++)); REG(pc) = GetAddress(); }, Is(I::JMP));
    Stage([this] { SetAddressLo(Read(REG(pc)++));                         }, Is(I::JSR));
    Stage([    ] { /* Internal operation. */                              }, Is(I::JSR));
    Stage([this] { Push(REG_HI(pc));                                      }, Is(I::JSR));
    Stage([this] { Push(REG_LO(pc));                                      }, Is(I::JSR));
    Stage([this] { SetAddressHi(Read(REG(pc)++)); REG(pc) = GetAddress(); }, Is(I::JSR));
    return;
  }

  // Stage the specified addressing mode.
  switch (GetAddressingMode()) {
  case A::ABS: Stage([this] { SetAddressLo(Read(REG(pc)++));                                                              }                          );
               Stage([this] { SetAddressHi(Read(REG(pc)++));                                                              }                          );
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ABX: Stage([this] { SetAddressLo(Read(REG(pc)++));                                                              }                          );
               Stage([this] { SetAddressHi(Read(REG(pc)++)); SetAddress(GetAddress(), REG(x));                            }                          );
               Stage([this] { if (CrossPage()) Read(FixHiByte(GetAddress())); return CrossPage() ? S::Continue : S::Skip; },              Is(M::READ));
               Stage([this] { if (CrossPage()) Read(FixHiByte(GetAddress())); else Read(GetAddress());                    },           IsNot(M::READ));
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ABY: Stage([this] { SetAddressLo(Read(REG(pc)++));                                                              }                          );
               Stage([this] { SetAddressHi(Read(REG(pc)++)); SetAddress(GetAddress(), REG(y));                            }                          );
               Stage([this] { if (CrossPage()) Read(FixHiByte(GetAddress())); return CrossPage() ? S::Continue : S::Skip; },              Is(M::READ));
               Stage([this] { if (CrossPage()) Read(FixHiByte(GetAddress())); else Read(GetAddress());                    },           IsNot(M::READ));
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZP0: Stage([this] { SetAddress(Read(REG(pc)++));                                                                }                          );
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZPX: Stage([this] { SetAddressLo(Read(REG(pc)++));                                                              }                          );
               Stage([this] { Read(REG(pc)); SetAddressLo(GetOffset() + REG(x));                                          }                          );
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZPY: Stage([this] { SetAddressLo(Read(REG(pc)++));                                                              }                          );
               Stage([this] { Read(REG(pc)); SetAddressLo(GetOffset() + REG(y));                                          }                          );
               Stage([this] { Fetch();                                                                                    }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                             }, Is(M::READ_MODIFY_WRITE)); break;
  default:
    break;
  }

  // Stage the specified instruction.
  switch (GetInstruction()) {
  case I::ADC: Stage([this] { REG(a) = Add(REG(a), Fetch());                                                              }               ); break;
  case I::AND: Stage([this] { REG(a) = And(REG(a), Fetch());                                                              }               ); break;
  case I::ASL: Stage([this] { REG(a) = ShiftL(REG(a), false);                                                             },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftL(Fetched(), false));                                              }, IsNot(A::ACC)); break;
  case I::BCC: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BCS: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BEQ: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BNE: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BMI: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BPL: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BVC: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BVS: Stage([    ] { /* Placeholder.*/                                                                           }               ); break;
  case I::BIT: Stage([this] { Bit(REG(a), Fetch());                                                                       }               ); break;
  case I::BRK: Stage([this] { REG(p) |= MSK(irq_disable); Read(REG(pc)++);                                                }               );
               Stage([this] { Push(REG_HI(pc));                                                                           }               );
               Stage([this] { Push(REG_LO(pc));                                                                           }               );
               Stage([this] { Push(REG(p) | MSK(brk_command));                                                            }               );
               Stage([this] { REG_LO(pc) = Read(MOS6502::kBRKAddress);                                                    }               );
               Stage([this] { REG_HI(pc) = Read(MOS6502::kBRKAddress + 1);                                                }               ); break;
  case I::CLC: Stage([this] { REG(p) &= ~MSK(carry);                                                                      }               ); break;
  case I::CLI: Stage([this] { REG(p) &= ~MSK(irq_disable);                                                                }               ); break;
  case I::CLD: Stage([this] { REG(p) &= ~MSK(decimal_mode);                                                               }               ); break;
  case I::CLV: Stage([this] { REG(p) &= ~MSK(overflow);                                                                   }               ); break;
  case I::CMP: Stage([this] { Cmp(REG(a), Fetch());                                                                       }               ); break;
  case I::CPX: Stage([this] { Cmp(REG(x), Fetch());                                                                       }               ); break;
  case I::CPY: Stage([this] { Cmp(REG(y), Fetch());                                                                       }               ); break;
  case I::DEC: Stage([this] { Write(GetAddress(), Decrement(Fetched()));                                                  }               ); break;
  case I::DEX: Stage([this] { REG(x) = Decrement(REG(x));                                                                 }               ); break;
  case I::DEY: Stage([this] { REG(y) = Decrement(REG(y));                                                                 }               ); break;
  case I::EOR: Stage([this] { REG(a) = Xor(REG(a), Fetch());                                                              }               ); break;
  case I::INC: Stage([this] { Write(GetAddress(), Increment(Fetched()));                                                  }               ); break;
  case I::INX: Stage([this] { REG(x) = Increment(REG(x));                                                                 }               ); break;
  case I::INY: Stage([this] { REG(y) = Increment(REG(y));                                                                 }               ); break;
  case I::LDA: Stage([this] { REG(a) = PassThrough(Fetch());                                                              }               ); break;
  case I::LDX: Stage([this] { REG(x) = PassThrough(Fetch());                                                              }               ); break;
  case I::LDY: Stage([this] { REG(y) = PassThrough(Fetch());                                                              }               ); break;
  case I::LSR: Stage([this] { REG(a) = ShiftR(REG(a), false);                                                             },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftR(Fetched(), false));                                              }, IsNot(A::ACC)); break;
  case I::NOP: Stage([    ] { /* Do nothing. */                                                                           }               ); break;
  case I::ORA: Stage([this] { REG(a) = Or(REG(a), Fetch());                                                               }               ); break;
  case I::PHA: Stage([this] { Read(REG(pc));                                                                              }               );
               Stage([this] { Push(REG(a));                                                                               }               ); break;
  case I::PHP: Stage([this] { Read(REG(pc));                                                                              }               );
               Stage([this] { Push(REG(p) | MSK(brk_command) | MSK(unused)); REG(p) &= ~(MSK(brk_command) | MSK(unused)); }               ); break;
  case I::PLA: Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                }               );
               Stage([this] { REG(a) = PassThrough(Pull());                                                               }               ); break;
  case I::PLP: Stage([this] { Read(REG(pc));                                                                              }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                }               );
               Stage([this] { REG(p) = Pull(); REG(p) |= MSK(unused);                                                     }               ); break;
  case I::ROL: Stage([this] { REG(a) = ShiftL(REG(a), true);                                                              },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftL(Fetched(), true));                                               }, IsNot(A::ACC)); break;
  case I::ROR: Stage([this] { REG(a) = ShiftR(REG(a), true);                                                              },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftR(Fetched(), true));                                               }, IsNot(A::ACC)); break;
  case I::RTI: Stage([this] { Read(REG(pc));                                                                              }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                }               );
               Stage([this] { REG(p) = Pull(); REG(p) &= ~(MSK(brk_command) | MSK(unused));                               }               );
               Stage([this] { REG_LO(pc) = Pull();                                                                        }               );
               Stage([this] { REG_HI(pc) = Pull();                                                                        }               ); break;
  case I::RTS: Stage([this] { Read(REG(pc));                                                                              }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                }               );
               Stage([this] { REG_LO(pc) = Pull();                                                                        }               );
               Stage([this] { REG_HI(pc) = Pull();                                                                        }               );
               Stage([this] { REG(pc)++;                                                                                  }               ); break;
  case I::SBC: Stage([this] { REG(a) = Sub(REG(a), Fetch());                                                              }               ); break;
  case I::SEC: Stage([this] { REG(p) |= MSK(carry);                                                                       }               ); break;
  case I::SEI: Stage([this] { REG(p) |= MSK(irq_disable);                                                                 }               ); break;
  case I::SED: Stage([this] { REG(p) |= MSK(decimal_mode);                                                                }               ); break;
  case I::STA: Stage([this] { Write(GetAddress(), REG(a));                                                                }               ); break;
  case I::STX: Stage([this] { Write(GetAddress(), REG(x));                                                                }               ); break;
  case I::STY: Stage([this] { Write(GetAddress(), REG(y));                                                                }               ); break;
  case I::TAX: Stage([this] { REG(x) = PassThrough(REG(a));                                                               }               ); break;
  case I::TAY: Stage([this] { REG(y) = PassThrough(REG(a));                                                               }               ); break;
  case I::TSX: Stage([this] { REG(x) = PassThrough(REG(s));                                                               }               ); break;
  case I::TXA: Stage([this] { REG(a) = PassThrough(REG(x));                                                               }               ); break;
  case I::TXS: Stage([this] { REG(s) = REG(x);                                                                            }               ); break;
  case I::TYA: Stage([this] { REG(a) = PassThrough(REG(y));                                                               }               ); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::RST() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

