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
#define FLG(x)    !!registers_->p.x
#define CLR(x)     !registers_->p.x
#define MSK(x)    registers_->p.x.mask
using A = AddressingMode;
using I = Instruction;
using M = MemoryAccess;
using S = Pipeline::Status;

/*
 * The high byte of the effective address may be invalid in indexed addressing
 * modes. Because the processor cannot undo a write to an invalid address, it
 * always reads from the address first.
 */
constexpr Address FixHiByte(Address address) noexcept {
  return address - static_cast<Address>(0x0100);
}

MOS6502::MOS6502(MOS6502::Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu}, stack_{registers, mmu}, alu_{registers} {}

MOS6502::~MOS6502() {}

void MOS6502::Tick() {
  if (ClearWhenCompleted()) {
//    if (nmi_) {
//      pipeline_ = create_nmi();
//      nmi_ = false;
//    } else {
    Next();
//    }
  } else {
    Execute();
  }
  ++context_.cycle;
}

/*
 * The following instruction timings are defined according to the following article.
 * [SEE] https://robinli.eu/f/6502_cpu.txt
 */
void MOS6502::Next() {
  // Parse next instruction.
  Parse();

  // Stage the specified addressing mode.
  switch (AddressingMode()) {
  case A::ACC: Stage(         /* Nothing to stage. */                                                                                                   ); break;
  case A::IMP: Stage(         /* Nothing to stage. */                                                                                                   ); break;
  case A::IMM: Stage(         /* Nothing to stage. */                                                                                                   ); break;
  case A::ABS: Stage([this] { Offset(Read(REG(pc)++));                                                                 }                                );
               Stage([this] { Page(Read(REG(pc)++)); REG(pc) = Address();                                              }, If(I::JMP)                    );
               Stage([    ] { /* Internal operation. */                                                                }, If(I::JSR)                    );
               Stage([this] { Push(REG_HI(pc));                                                                        }, If(I::JSR)                    );
               Stage([this] { Push(REG_LO(pc));                                                                        }, If(I::JSR)                    );
               Stage([this] { Page(Read(REG(pc)++)); REG(pc) = Address();                                              }, If(I::JSR)                    );
               Stage([this] { Page(Read(REG(pc)++));                                                                   }, IfNot(I::JMP) && IfNot(I::JSR));
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  case A::ABX: Stage([this] { Offset(Read(REG(pc)++));                                                                 }                                );
               Stage([this] { Page(Read(REG(pc)++)); Address(Address(), REG(x));                                       }                                );
               Stage([this] { if (CrossPage()) Read(FixHiByte(Address())); return CrossPage() ? S::Continue : S::Skip; }, If(M::READ)                   );
               Stage([this] { if (CrossPage()) Read(FixHiByte(Address())); else Read(Address());                       }, IfNot(M::READ)                );
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  case A::ABY: Stage([this] { Offset(Read(REG(pc)++));                                                                 }                                );
               Stage([this] { Page(Read(REG(pc)++)); Address(Address(), REG(y));                                       }                                );
               Stage([this] { if (CrossPage()) Read(FixHiByte(Address())); return CrossPage() ? S::Continue : S::Skip; }, If(M::READ)                   );
               Stage([this] { if (CrossPage()) Read(FixHiByte(Address())); else Read(Address());                       }, IfNot(M::READ)                );
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  case A::ZP0: Stage([this] { Address(Read(REG(pc)++));                                                                }                                );
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  case A::ZPX: Stage([this] { Offset(Read(REG(pc)++));                                                                 }                                );
               Stage([this] { Read(REG(pc)); Offset(Offset() + REG(x));                                                }                                );
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  case A::ZPY: Stage([this] { Offset(Read(REG(pc)++));                                                                 }                                );
               Stage([this] { Read(REG(pc)); Offset(Offset() + REG(y));                                                }                                );
               Stage([this] { Fetch();                                                                                 }, If(M::READ_MODIFY_WRITE)      );
               Stage([this] { Write(Address(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE)      ); break;
  default    : NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid addressing mode specified to Fetch", Opcode()));
  }

  // Stage the specified instruction.
  switch (Instruction()) {
  case I::ADC: Stage([this] { REG(a) = Add(REG(a), Fetch());                                                               }               ); break;
  case I::AND: Stage([this] { REG(a) = And(REG(a), Fetch());                                                               }               ); break;
  case I::ASL: Stage([this] { REG(a) = ShiftL(REG(a), false);                                                              }, If(A::ACC)   );
               Stage([this] { Write(Address(), ShiftL(Fetched(), false));                                                  }, IfNot(A::ACC)); break;
  case I::BCC: Stage([this] { if (!CLR(carry)) REG(pc)++; return !CLR(carry) ? S::Stop : S::Continue;                      }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BCS: Stage([this] { if (!FLG(carry)) REG(pc)++; return !FLG(carry) ? S::Stop : S::Continue;                      }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BEQ: Stage([this] { if (!FLG(zero)) REG(pc)++; return !FLG(zero) ? S::Stop : S::Continue;                        }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BNE: Stage([this] { if (!CLR(zero)) REG(pc)++; return !CLR(zero) ? S::Stop : S::Continue;                        }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BMI: Stage([this] { if (!FLG(negative)) REG(pc)++; return !FLG(negative) ? S::Stop : S::Continue;                }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BPL: Stage([this] { if (!CLR(negative)) REG(pc)++; return !CLR(negative) ? S::Stop : S::Continue;                }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BVC: Stage([this] { if (!CLR(overflow)) REG(pc)++; return !CLR(overflow) ? S::Stop : S::Continue;                }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BVS: Stage([this] { if (!FLG(overflow)) REG(pc)++; return !FLG(overflow) ? S::Stop : S::Continue;                }               );
               Stage([this] { Offset(REG(pc)++); FixPage(); Branch(Address()); return CrossPage() ? S::Continue : S::Stop; }               );
               Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::BIT: Stage([this] { Bit(REG(a), Fetch());                                                                        }               ); break;
  case I::BRK: Stage([this] { REG(p) |= MSK(irq_disable); Read(REG(pc)++);                                                 }               );
               Stage([this] { Push(REG_HI(pc));                                                                            }               );
               Stage([this] { Push(REG_LO(pc));                                                                            }               );
               Stage([this] { Push(REG(p) | MSK(brk_command));                                                             }               );
               Stage([this] { REG_LO(pc) = Read(MOS6502::kBRKAddress);                                                     }               );
               Stage([this] { REG_HI(pc) = Read(MOS6502::kBRKAddress + 1);                                                 }               ); break;
  case I::CLC: Stage([this] { REG(p) &= ~MSK(carry);                                                                       }               ); break;
  case I::CLI: Stage([this] { REG(p) &= ~MSK(irq_disable);                                                                 }               ); break;
  case I::CLD: Stage([this] { REG(p) &= ~MSK(decimal_mode);                                                                }               ); break;
  case I::CLV: Stage([this] { REG(p) &= ~MSK(overflow);                                                                    }               ); break;
  case I::CMP: Stage([this] { Cmp(REG(a), Fetch());                                                                        }               ); break;
  case I::CPX: Stage([this] { Cmp(REG(x), Fetch());                                                                        }               ); break;
  case I::CPY: Stage([this] { Cmp(REG(y), Fetch());                                                                        }               ); break;
  case I::DEC: Stage([this] { Write(Address(), Decrement(Fetched()));                                                      }               ); break;
  case I::DEX: Stage([this] { REG(x) = Decrement(REG(x));                                                                  }               ); break;
  case I::DEY: Stage([this] { REG(y) = Decrement(REG(y));                                                                  }               ); break;
  case I::EOR: Stage([this] { REG(a) = Xor(REG(a), Fetch());                                                               }               ); break;
  case I::INC: Stage([this] { Write(Address(), Increment(Fetched()));                                                      }               ); break;
  case I::INX: Stage([this] { REG(x) = Increment(REG(x));                                                                  }               ); break;
  case I::INY: Stage([this] { REG(y) = Increment(REG(y));                                                                  }               ); break;
  case I::JMP: Stage(         /* Nothing to stage. */                                                                                      ); break;
  case I::JSR: Stage(         /* Nothing to stage. */                                                                                      ); break;
  case I::LDA: Stage([this] { REG(a) = PassThrough(Fetch());                                                               }               ); break;
  case I::LDX: Stage([this] { REG(x) = PassThrough(Fetch());                                                               }               ); break;
  case I::LDY: Stage([this] { REG(y) = PassThrough(Fetch());                                                               }               ); break;
  case I::LSR: Stage([this] { REG(a) = ShiftR(REG(a), false);                                                              }, If(A::ACC)   );
               Stage([this] { Write(Address(), ShiftR(Fetched(), false));                                                  }, IfNot(A::ACC)); break;
  case I::NOP: Stage([    ] { /* Do nothing. */                                                                            }               ); break;
  case I::ORA: Stage([this] { REG(a) = Or(REG(a), Fetch());                                                                }               ); break;
  case I::PHA: Stage([this] { Read(REG(pc));                                                                               }               );
               Stage([this] { Push(REG(a));                                                                                }               ); break;
  case I::PHP: Stage([this] { Read(REG(pc));                                                                               }               );
               Stage([this] { Push(REG(p) | MSK(brk_command) | MSK(unused)); REG(p) &= ~(MSK(brk_command) | MSK(unused));  }               ); break;
  case I::PLA: Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                 }               );
               Stage([this] { REG(a) = PassThrough(Pull());                                                                }               ); break;
  case I::PLP: Stage([this] { Read(REG(pc));                                                                               }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                 }               );
               Stage([this] { REG(p) = Pull(); REG(p) |= MSK(unused);                                                      }               ); break;
  case I::ROL: Stage([this] { REG(a) = ShiftL(REG(a), true);                                                               }, If(A::ACC)   );
               Stage([this] { Write(Address(), ShiftL(Fetched(), true));                                                   }, IfNot(A::ACC)); break;
  case I::ROR: Stage([this] { REG(a) = ShiftR(REG(a), true);                                                               }, If(A::ACC)   );
               Stage([this] { Write(Address(), ShiftR(Fetched(), true));                                                   }, IfNot(A::ACC)); break;
  case I::RTI: Stage([this] { Read(REG(pc));                                                                               }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                 }               );
               Stage([this] { REG(p) = Pull(); REG(p) &= ~(MSK(brk_command) | MSK(unused));                                }               );
               Stage([this] { REG_LO(pc) = Pull();                                                                         }               );
               Stage([this] { REG_HI(pc) = Pull();                                                                         }               ); break;
  case I::RTS: Stage([this] { Read(REG(pc));                                                                               }               );
               Stage([    ] { /* Increment stack pointer. Done in Pull. */                                                 }               );
               Stage([this] { REG_LO(pc) = Pull();                                                                         }               );
               Stage([this] { REG_HI(pc) = Pull();                                                                         }               );
               Stage([this] { REG(pc)++;                                                                                   }               ); break;
  case I::SBC: Stage([this] { REG(a) = Sub(REG(a), Fetch());                                                               }               ); break;
  case I::SEC: Stage([this] { REG(p) |= MSK(carry);                                                                        }               ); break;
  case I::SEI: Stage([this] { REG(p) |= MSK(irq_disable);                                                                  }               ); break;
  case I::SED: Stage([this] { REG(p) |= MSK(decimal_mode);                                                                 }               ); break;
  case I::STA: Stage([this] { Write(Address(), REG(a));                                                                    }               ); break;
  case I::STX: Stage([this] { Write(Address(), REG(x));                                                                    }               ); break;
  case I::STY: Stage([this] { Write(Address(), REG(y));                                                                    }               ); break;
  case I::TAX: Stage([this] { REG(x) = PassThrough(REG(a));                                                                }               ); break;
  case I::TAY: Stage([this] { REG(y) = PassThrough(REG(a));                                                                }               ); break;
  case I::TSX: Stage([this] { REG(x) = PassThrough(REG(s));                                                                }               ); break;
  case I::TXA: Stage([this] { REG(a) = PassThrough(REG(x));                                                                }               ); break;
  case I::TXS: Stage([this] { REG(s) = REG(x);                                                                             }               ); break;
  case I::TYA: Stage([this] { REG(a) = PassThrough(REG(y));                                                                }               ); break;
  default    : NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", Opcode()));
  }
}

void MOS6502::RST() noexcept {}

void MOS6502::IRQ() noexcept {}

void MOS6502::NMI() noexcept {}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

