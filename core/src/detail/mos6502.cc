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

#define REGISTER(x)    registers_->x.value
#define REGISTER_LO(x) registers_->x.lo
#define REGISTER_HI(x) registers_->x.hi
#define FLAG(x)        registers_->p.x
#define MASK(x)        registers_->p.x.mask
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
    Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                              }, Is(I::JMP));
    Stage([this] { SetAddressHi(Read(REGISTER(pc)++)); REGISTER(pc) = GetAddress(); }, Is(I::JMP));
    Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                              }, Is(I::JSR));
    Stage([    ] { /* Internal operation. */                                        }, Is(I::JSR));
    Stage([this] { Push(REGISTER_HI(pc));                                           }, Is(I::JSR));
    Stage([this] { Push(REGISTER_LO(pc));                                           }, Is(I::JSR));
    Stage([this] { SetAddressHi(Read(REGISTER(pc)++)); REGISTER(pc) = GetAddress(); }, Is(I::JSR));
    return;
  }

  // Stage the specified addressing mode.
  switch (GetAddressingMode()) {
  case A::ABS: Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { SetAddressHi(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ABX: Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { SetAddressHi(Read(REGISTER(pc)++)); SetAddress(GetAddress(), REGISTER(x));                                              }                          );
               Stage([this] { if (IsPageCrossed()) Read(GetAddress() - static_cast<Address>(0x0100)); return IsPageCrossed() ? S::Continue : S::Skip; },              Is(M::READ));
               Stage([this] { if (IsPageCrossed()) Read(GetAddress() - static_cast<Address>(0x0100)); else Read(GetAddress());                        },           IsNot(M::READ));
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ABY: Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { SetAddressHi(Read(REGISTER(pc)++)); SetAddress(GetAddress(), REGISTER(y));                                              }                          );
               Stage([this] { if (IsPageCrossed()) Read(GetAddress() - static_cast<Address>(0x0100)); return IsPageCrossed() ? S::Continue : S::Skip; },              Is(M::READ));
               Stage([this] { if (IsPageCrossed()) Read(GetAddress() - static_cast<Address>(0x0100)); else Read(GetAddress());                        },           IsNot(M::READ));
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZP0: Stage([this] { SetAddress(Read(REGISTER(pc)++));                                                                                       }                          );
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZPX: Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { Read(REGISTER(pc)); SetAddressLo(GetOffset() + REGISTER(x));                                                            }                          );
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  case A::ZPY: Stage([this] { SetAddressLo(Read(REGISTER(pc)++));                                                                                     }                          );
               Stage([this] { Read(REGISTER(pc)); SetAddressLo(GetOffset() + REGISTER(y));                                                            }                          );
               Stage([this] { Fetch();                                                                                                                }, Is(M::READ_MODIFY_WRITE));
               Stage([this] { Write(GetAddress(), Fetched());                                                                                         }, Is(M::READ_MODIFY_WRITE)); break;
  default:
    break;
  }

  // Stage the specified instruction.
  switch (GetInstruction()) {
  case I::ADC: Stage([this] { REGISTER(a) = Add(REGISTER(a), Fetch());                                                                  }               ); break;
  case I::AND: Stage([this] { REGISTER(a) = And(REGISTER(a), Fetch());                                                                  }               ); break;
  case I::ASL: Stage([this] { REGISTER(a) = ShiftL(REGISTER(a), false);                                                                 },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftL(Fetched(), false));                                                            }, IsNot(A::ACC)); break;
  case I::BCC: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BCS: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BEQ: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BNE: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BMI: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BPL: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BVC: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BVS: Stage([    ] { /* Placeholder.*/                                                                                         }               ); break;
  case I::BIT: Stage([this] { Bit(REGISTER(a), Fetch());                                                                                }               ); break;
  case I::BRK: Stage([this] { REGISTER(p) |= MASK(irq_disable); Read(REGISTER(pc)++);                                                   }               );
               Stage([this] { Push(REGISTER_HI(pc));                                                                                    }               );
               Stage([this] { Push(REGISTER_LO(pc));                                                                                    }               );
               Stage([this] { Push(REGISTER(p) | MASK(brk_command));                                                                    }               );
               Stage([this] { REGISTER_LO(pc) = Read(MOS6502::kBRKAddress);                                                             }               );
               Stage([this] { REGISTER_HI(pc) = Read(MOS6502::kBRKAddress + 1);                                                         }               ); break;
  case I::CLC: Stage([this] { REGISTER(p) &= ~MASK(carry);                                                                              }               ); break;
  case I::CLI: Stage([this] { REGISTER(p) &= ~MASK(irq_disable);                                                                        }               ); break;
  case I::CLD: Stage([this] { REGISTER(p) &= ~MASK(decimal_mode);                                                                       }               ); break;
  case I::CLV: Stage([this] { REGISTER(p) &= ~MASK(overflow);                                                                           }               ); break;
  case I::CMP: Stage([this] { Cmp(REGISTER(a), Fetch());                                                                                }               ); break;
  case I::CPX: Stage([this] { Cmp(REGISTER(x), Fetch());                                                                                }               ); break;
  case I::CPY: Stage([this] { Cmp(REGISTER(y), Fetch());                                                                                }               ); break;
  case I::DEC: Stage([this] { Write(GetAddress(), Decrement(Fetched()));                                                                }               ); break;
  case I::DEX: Stage([this] { REGISTER(x) = Decrement(REGISTER(x));                                                                     }               ); break;
  case I::DEY: Stage([this] { REGISTER(y) = Decrement(REGISTER(y));                                                                     }               ); break;
  case I::EOR: Stage([this] { REGISTER(a) = Xor(REGISTER(a), Fetch());                                                                  }               ); break;
  case I::INC: Stage([this] { Write(GetAddress(), Increment(Fetched()));                                                                }               ); break;
  case I::INX: Stage([this] { REGISTER(x) = Increment(REGISTER(x));                                                                     }               ); break;
  case I::INY: Stage([this] { REGISTER(y) = Increment(REGISTER(y));                                                                     }               ); break;
  case I::LDA: Stage([this] { REGISTER(a) = PassThrough(Fetch());                                                                       }               ); break;
  case I::LDX: Stage([this] { REGISTER(x) = PassThrough(Fetch());                                                                       }               ); break;
  case I::LDY: Stage([this] { REGISTER(y) = PassThrough(Fetch());                                                                       }               ); break;
  case I::LSR: Stage([this] { REGISTER(a) = ShiftR(REGISTER(a), false);                                                                 },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftR(Fetched(), false));                                                            }, IsNot(A::ACC)); break;
  case I::NOP: Stage([    ] { /* Do nothing. */                                                                                         }               ); break;
  case I::ORA: Stage([this] { REGISTER(a) = Or(REGISTER(a), Fetch());                                                                   }               ); break;
  case I::PHA: Stage([this] { Read(REGISTER(pc));                                                                                       }               );
               Stage([this] { Push(REGISTER(a));                                                                                        }               ); break;
  case I::PHP: Stage([this] { Read(REGISTER(pc));                                                                                       }               );
               Stage([this] { Push(REGISTER(p) | MASK(brk_command) | MASK(unused)); REGISTER(p) &= ~(MASK(brk_command) | MASK(unused)); }               ); break;
  case I::PLA: Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                              }               );
               Stage([this] { REGISTER(a) = PassThrough(Pull());                                                                        }               ); break;
  case I::PLP: Stage([this] { Read(REGISTER(pc));                                                                                       }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                              }               );
               Stage([this] { REGISTER(p) = Pull(); REGISTER(p) |= MASK(unused);                                                        }               ); break;
  case I::ROL: Stage([this] { REGISTER(a) = ShiftL(REGISTER(a), true);                                                                  },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftL(Fetched(), true));                                                             }, IsNot(A::ACC)); break;
  case I::ROR: Stage([this] { REGISTER(a) = ShiftR(REGISTER(a), true);                                                                  },    Is(A::ACC));
               Stage([this] { Write(GetAddress(), ShiftR(Fetched(), true));                                                             }, IsNot(A::ACC)); break;
  case I::RTI: Stage([this] { Read(REGISTER(pc));                                                                                       }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                              }               );
               Stage([this] { REGISTER(p) = Pull(); REGISTER(p) &= ~(MASK(brk_command) | MASK(unused));                                 }               );
               Stage([this] { REGISTER_LO(pc) = Pull();                                                                                 }               );
               Stage([this] { REGISTER_HI(pc) = Pull();                                                                                 }               ); break;
  case I::RTS: Stage([this] { Read(REGISTER(pc));                                                                                       }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                              }               );
               Stage([this] { REGISTER_LO(pc) = Pull();                                                                                 }               );
               Stage([this] { REGISTER_HI(pc) = Pull();                                                                                 }               );
               Stage([this] { REGISTER(pc)++;                                                                                           }               ); break;
  case I::SBC: Stage([this] { REGISTER(a) = Sub(REGISTER(a), Fetch());                                                                  }               ); break;
  case I::SEC: Stage([this] { REGISTER(p) |= MASK(carry);                                                                               }               ); break;
  case I::SEI: Stage([this] { REGISTER(p) |= MASK(irq_disable);                                                                         }               ); break;
  case I::SED: Stage([this] { REGISTER(p) |= MASK(decimal_mode);                                                                        }               ); break;
  case I::STA: Stage([this] { Write(GetAddress(), REGISTER(a));                                                                         }               ); break;
  case I::STX: Stage([this] { Write(GetAddress(), REGISTER(x));                                                                         }               ); break;
  case I::STY: Stage([this] { Write(GetAddress(), REGISTER(y));                                                                         }               ); break;
  case I::TAX: Stage([this] { REGISTER(x) = PassThrough(REGISTER(a));                                                                   }               ); break;
  case I::TAY: Stage([this] { REGISTER(y) = PassThrough(REGISTER(a));                                                                   }               ); break;
  case I::TSX: Stage([this] { REGISTER(x) = PassThrough(REGISTER(s));                                                                   }               ); break;
  case I::TXA: Stage([this] { REGISTER(a) = PassThrough(REGISTER(x));                                                                   }               ); break;
  case I::TXS: Stage([this] { REGISTER(s) = REGISTER(x);                                                                                }               ); break;
  case I::TYA: Stage([this] { REGISTER(a) = PassThrough(REGISTER(y));                                                                   }               ); break;
  default: NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", GetOpcode()));
  }
}

void MOS6502::RST() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

