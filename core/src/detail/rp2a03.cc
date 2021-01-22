/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "nesdev/core/cpu.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"
#include "pipeline.h"
#include "detail/rp2a03.h"

namespace nesdev {
namespace core {
namespace detail {

#define REG(x)    registers_->x.value
#define REG_LO(x) registers_->x.lo
#define REG_HI(x) registers_->x.hi
#define MSK(x)    registers_->p.x.mask
using A = AddressingMode;
using I = Instruction;
using M = MemoryAccess;
using S = Pipeline::Status;

RP2A03::RP2A03(RP2A03::Registers* const registers, MMU* const mmu)
  : registers_{registers}, mmu_{mmu}, stack_{registers, mmu}, alu_{registers} {}

RP2A03::~RP2A03() {}

void RP2A03::Tick() {
  if (ClearWhenCompleted()) RST() || IRQ() || NMI() || Next();
  else Execute();
  ++context_.cycle;
}

/*
 * The following instruction timings are defined according to the following article.
 * [SEE] https://robinli.eu/f/6502_cpu.txt
 *
 * NOTE: The following opcodes are invelid in MOS6502 archetecture, but still
 *       capable to handle each addressing modes and instructions, so will not
 *       throw exceptions.
 *       {0x1A, {Instruction::INC, AddressingMode::ACC, MemoryAccess::READ_MODIFY_WRITE}} // ***65C02-***
 *       {0x3A, {Instruction::DEC, AddressingMode::ACC, MemoryAccess::READ_MODIFY_WRITE}} // ***65C02-***
 *       {0x3C, {Instruction::BIT, AddressingMode::ABX, MemoryAccess::READ             }} // ***65C02-***
 *       {0x89, {Instruction::BIT, AddressingMode::IMM, MemoryAccess::READ             }} // ***65C02-***
 */
bool RP2A03::Next() {
  // Parse next instruction.
  Parse();
  // Stage the specified addressing mode.
  switch (AddrMode()) {
  case A::ACC:
    Stage(/* Nothing to stage. */);
    break;
  case A::IMP:
    Stage(/* Nothing to stage. */);
    break;
  case A::IMM:
    Stage(/* Nothing to stage. */);
    break;
  case A::REL:
    Stage(/* Nothing to stage. */);
    break;
  case A::ABS:
    Stage([this] { AddrLo(Read(REG(pc)++));                   }                                );
    Stage([this] { AddrHi(Read(REG(pc)++)); REG(pc) = Addr(); }, If(I::JMP)                    );
    Stage([    ] { /* Internal operation. */                  }, If(I::JSR)                    );
    Stage([this] { Push(REG_HI(pc));                          }, If(I::JSR)                    );
    Stage([this] { Push(REG_LO(pc));                          }, If(I::JSR)                    );
    Stage([this] { AddrHi(Read(REG(pc)++)); REG(pc) = Addr(); }, If(I::JSR)                    );
    Stage([this] { AddrHi(Read(REG(pc)++));                   }, IfNot(I::JMP) && IfNot(I::JSR));
    Stage([this] { Fetch();                                   }, If(M::READ_MODIFY_WRITE)      );
    Stage([this] { Write(Addr(), Fetched());                  }, If(M::READ_MODIFY_WRITE)      );
    break;
  case A::ABX:
    Stage([this] { AddrLo(Read(REG(pc)++));                                                              }                          );
    Stage([this] { AddrHi(Read(REG(pc)++)); Addr(Addr(), REG(x));                                        }                          );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); return CrossPage() ? S::Continue : S::Skip; }, If(M::READ)             );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); else Read(Addr());                          }, IfNot(M::READ)          );
    Stage([this] { Fetch();                                                                              }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE));
    break;
  case A::ABY:
    Stage([this] { AddrLo(Read(REG(pc)++));                                                              }                          );
    Stage([this] { AddrHi(Read(REG(pc)++)); Addr(Addr(), REG(y));                                        }                          );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); return CrossPage() ? S::Continue : S::Skip; }, If(M::READ)             );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); else Read(Addr());                          }, IfNot(M::READ)          );
    Stage([this] { Fetch();                                                                              }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE));
    break;
  case A::ZP0:
    Stage([this] { Addr(Read(REG(pc)++));    }                          );
    Stage([this] { Fetch();                  }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched()); }, If(M::READ_MODIFY_WRITE));
    break;
  case A::ZPX:
    Stage([this] { Addr(Read(REG(pc)++));                    }                          );
    Stage([this] { Read(REG(pc)); AddrLo(AddrLo() + REG(x)); }                          );
    Stage([this] { Fetch();                                  }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                 }, If(M::READ_MODIFY_WRITE));
    break;
  case A::ZPY:
    Stage([this] { Addr(Read(REG(pc)++));                    }                          );
    Stage([this] { Read(REG(pc)); AddrLo(AddrLo() + REG(y)); }                          );
    Stage([this] { Fetch();                                  }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                 }, If(M::READ_MODIFY_WRITE));
    break;
  case A::IND:
    Stage([this] { PtrLo(Read(REG(pc)++));                                                                            });
    Stage([this] { PtrHi(Read(REG(pc)++));                                                                            });
    Stage([this] { AddrLo(Read(Ptr()));                                                                               });
    Stage([this] { if (PtrLo() == 0xFF) AddrHi(Read(Ptr() & 0xFF00)); else AddrHi(Read(Ptr() + 1)); REG(pc) = Addr(); });
    break;
  case A::IZX:
    Stage([this] { Ptr(Read(REG(pc)++));                                   }                          );
    Stage([this] { Read(Ptr());                                            }                          );
    Stage([this] { AddrLo(Read(Ptr() + static_cast<Address>(REG(x))));     }                          );
    Stage([this] { AddrHi(Read(Ptr() + static_cast<Address>(REG(x)) + 1)); }                          );
    Stage([this] { Fetch();                                                }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                               }, If(M::READ_MODIFY_WRITE));
    break;
  case A::IZY:
    Stage([this] { Ptr(Read(REG(pc)++));                                                                 }                          );
    Stage([this] { AddrLo(Read(Ptr()));                                                                  }                          );
    Stage([this] { AddrHi(Read(Ptr() + 1)); Addr(Addr(), REG(y));                                        }                          );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); return CrossPage() ? S::Continue : S::Skip; }, If(M::READ)             );
    Stage([this] { if (CrossPage()) Read(FixHiByte(Addr())); else Read(Addr());                          }, IfNot(M::READ)          );
    Stage([this] { Fetch();                                                                              }, If(M::READ_MODIFY_WRITE));
    Stage([this] { Write(Addr(), Fetched());                                                             }, If(M::READ_MODIFY_WRITE));
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid addressing mode specified to Fetch", Op()));
  }
  // Stage the specified instruction.
  switch (Inst()) {
  case I::ADC:
    Stage([this] { REG(a) = Add(REG(a), Fetch()); });
    break;
  case I::AND:
    Stage([this] { REG(a) = And(REG(a), Fetch()); });
    break;
  case I::ASL:
    Stage([this] { REG(a) = ShiftL(REG(a), false);          }, If(A::ACC)   );
    Stage([this] { Write(Addr(), ShiftL(Fetched(), false)); }, IfNot(A::ACC));
    break;
  case I::BCC:
    Stage([this] { REG(pc)++;                                                                              }, IfCarry()   );
    Stage([    ] { /* Check if carry is clear, done in staging phase here. */                              }, IfNotCarry());
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfNotCarry());
    Stage([    ] { /* Do nothing. */                                                                       }, IfNotCarry());
    break;
  case I::BCS:
    Stage([this] { REG(pc)++;                                                                              }, IfNotCarry());
    Stage([    ] { /* Check if carry is set, done in staging phase here. */                                }, IfCarry()   );
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfCarry()   );
    Stage([    ] { /* Do nothing. */                                                                       }, IfCarry()   );
    break;
  case I::BEQ:
    Stage([this] { REG(pc)++;                                                                              }, IfNotZero());
    Stage([    ] { /* Check if zero is set, done in staging phase here. */                                 }, IfZero()   );
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfZero()   );
    Stage([    ] { /* Do nothing. */                                                                       }, IfZero()   );
    break;
  case I::BNE:
    Stage([this] { REG(pc)++;                                                                              }, IfZero()   );
    Stage([    ] { /* Check if zero is clear, done in staging phase here. */                               }, IfNotZero());
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfNotZero());
    Stage([    ] { /* Do nothing. */                                                                       }, IfNotZero());
    break;
  case I::BMI:
    Stage([this] { REG(pc)++;                                                                              }, IfNotNegative());
    Stage([    ] { /* Check if negative is set, done in staging phase here. */                             }, IfNegative()   );
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfNegative()   );
    Stage([    ] { /* Do nothing. */                                                                       }, IfNegative()   );
    break;
  case I::BPL:
    Stage([this] { REG(pc)++;                                                                              }, IfNegative()   );
    Stage([    ] { /* Check if negative is clear, done in staging phase here. */                           }, IfNotNegative());
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfNotNegative());
    Stage([    ] { /* Do nothing. */                                                                       }, IfNotNegative());
    break;
  case I::BVC:
    Stage([this] { REG(pc)++;                                                                              }, IfOverflow()   );
    Stage([    ] { /* Check if negative is clear, done in staging phase here. */                           }, IfNotOverflow());
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfNotOverflow());
    Stage([    ] { /* Do nothing. */                                                                       }, IfNotOverflow());
    break;
  case I::BVS:
    Stage([this] { REG(pc)++;                                                                              }, IfNotOverflow());
    Stage([    ] { /* Check if negative is set, done in staging phase here. */                             }, IfOverflow()   );
    Stage([this] { Addr(REG(pc)++); FixPage(); Branch(Addr()); return CrossPage() ? S::Continue : S::Stop; }, IfOverflow()   );
    Stage([    ] { /* Do nothing. */                                                                       }, IfOverflow()   );
    break;
  case I::BIT:
    Stage([this] { Bit(REG(a), Fetch()); });
    break;
  case I::BRK:
    Stage([this] { REG(p) |= MSK(irq_disable); Read(REG(pc)++); });
    Stage([this] { Push(REG_HI(pc));                            });
    Stage([this] { Push(REG_LO(pc));                            });
    Stage([this] { Push(REG(p) | MSK(brk_command));             });
    Stage([this] { REG_LO(pc) = Read(RP2A03::kBRKAddress);      });
    Stage([this] { REG_HI(pc) = Read(RP2A03::kBRKAddress + 1);  });
    break;
  case I::CLC:
    Stage([this] { REG(p) &= ~MSK(carry); });
    break;
  case I::CLI:
    Stage([this] { REG(p) &= ~MSK(irq_disable); });
    break;
  case I::CLD:
    Stage([this] { REG(p) &= ~MSK(decimal_mode); });
    break;
  case I::CLV:
    Stage([this] { REG(p) &= ~MSK(overflow); });
    break;
  case I::CMP:
    Stage([this] { Cmp(REG(a), Fetch()); });
    break;
  case I::CPX:
    Stage([this] { Cmp(REG(x), Fetch()); });
    break;
  case I::CPY:
    Stage([this] { Cmp(REG(y), Fetch()); });
    break;
  case I::DEC:
    Stage([this] { Write(Addr(), Decrement(Fetched())); });
    break;
  case I::DEX:
    Stage([this] { REG(x) = Decrement(REG(x)); });
    break;
  case I::DEY:
    Stage([this] { REG(y) = Decrement(REG(y)); });
    break;
  case I::EOR:
    Stage([this] { REG(a) = Xor(REG(a), Fetch()); });
    break;
  case I::INC:
    Stage([this] { Write(Addr(), Increment(Fetched())); });
    break;
  case I::INX:
    Stage([this] { REG(x) = Increment(REG(x)); });
    break;
  case I::INY:
    Stage([this] { REG(y) = Increment(REG(y)); });
    break;
  case I::JMP:
    Stage(/* Nothing to stage. */);
    break;
  case I::JSR:
    Stage(/* Nothing to stage. */);
    break;
  case I::LDA:
    Stage([this] { REG(a) = PassThrough(Fetch()); });
    break;
  case I::LDX:
    Stage([this] { REG(x) = PassThrough(Fetch()); });
    break;
  case I::LDY:
    Stage([this] { REG(y) = PassThrough(Fetch()); });
    break;
  case I::LSR:
    Stage([this] { REG(a) = ShiftR(REG(a), false);          }, If(A::ACC)   );
    Stage([this] { Write(Addr(), ShiftR(Fetched(), false)); }, IfNot(A::ACC));
    break;
  case I::NOP:
    Stage([    ] { /* Do nothing. */ });
    break;
  case I::ORA:
    Stage([this] { REG(a) = Or(REG(a), Fetch()); });
    break;
  case I::PHA:
    Stage([this] { Read(REG(pc)); });
    Stage([this] { Push(REG(a));  });
    break;
  case I::PHP:
    Stage([this] { Read(REG(pc));                                                                              });
    Stage([this] { Push(REG(p) | MSK(brk_command) | MSK(unused)); REG(p) &= ~(MSK(brk_command) | MSK(unused)); });
    break;
  case I::PLA:
    Stage([this] { Read(REG(pc));                               });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REG(a) = PassThrough(Pull());                });
    break;
  case I::PLP:
    Stage([this] { Read(REG(pc));                               });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REG(p) = Pull(); REG(p) |= MSK(unused);      });
    break;
  case I::ROL:
    Stage([this] { REG(a) = ShiftL(REG(a), true);          }, If(A::ACC)   );
    Stage([this] { Write(Addr(), ShiftL(Fetched(), true)); }, IfNot(A::ACC));
    break;
  case I::ROR:
    Stage([this] { REG(a) = ShiftR(REG(a), true);          }, If(A::ACC)   );
    Stage([this] { Write(Addr(), ShiftR(Fetched(), true)); }, IfNot(A::ACC));
    break;
  case I::RTI:
    Stage([this] { Read(REG(pc));                                                });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */                  });
    Stage([this] { REG(p) = Pull(); REG(p) &= ~(MSK(brk_command) | MSK(unused)); });
    Stage([this] { REG_LO(pc) = Pull();                                          });
    Stage([this] { REG_HI(pc) = Pull();                                          });
    break;
  case I::RTS:
    Stage([this] { Read(REG(pc));                               });
    Stage([    ] { /* Increment stack pointer. Done in Pull. */ });
    Stage([this] { REG_LO(pc) = Pull();                         });
    Stage([this] { REG_HI(pc) = Pull();                         });
    Stage([this] { REG(pc)++;                                   });
    break;
  case I::SBC:
    Stage([this] { REG(a) = Sub(REG(a), Fetch()); });
    break;
  case I::SEC:
    Stage([this] { REG(p) |= MSK(carry); });
    break;
  case I::SEI:
    Stage([this] { REG(p) |= MSK(irq_disable); });
    break;
  case I::SED:
    Stage([this] { REG(p) |= MSK(decimal_mode); });
    break;
  case I::STA:
    Stage([this] { Write(Addr(), REG(a)); });
    break;
  case I::STX:
    Stage([this] { Write(Addr(), REG(x)); });
    break;
  case I::STY:
    Stage([this] { Write(Addr(), REG(y)); });
    break;
  case I::TAX:
    Stage([this] { REG(x) = PassThrough(REG(a)); });
    break;
  case I::TAY:
    Stage([this] { REG(y) = PassThrough(REG(a)); });
    break;
  case I::TSX:
    Stage([this] { REG(x) = PassThrough(REG(s)); });
    break;
  case I::TXA:
    Stage([this] { REG(a) = PassThrough(REG(x)); });
    break;
  case I::TXS:
    Stage([this] { REG(s) = REG(x); });
    break;
  case I::TYA:
    Stage([this] { REG(a) = PassThrough(REG(y)); });
    break;
  default:
    NESDEV_CORE_THROW(InvalidOpcode::Occur("Invalid instruction specified to Fetch", Op()));
  }
  // Return true, just for coding comfortability.
  return true;
}

bool RP2A03::RST() noexcept {
  Stage([this] { AddrLo(Read(RP2A03::kRSTAddress));                       }, IfRST());
  Stage([this] { AddrHi(Read(RP2A03::kRSTAddress + 1)); REG(pc) = Addr(); }, IfRST());
  Stage([this] { REG(a) = 0x00;                                           }, IfRST());
  Stage([this] { REG(x) = 0x00;                                           }, IfRST());
  Stage([this] { REG(y) = 0x00;                                           }, IfRST());
  Stage([this] { REG(s) = Stack::kHead;                                   }, IfRST());
  Stage([this] { REG(p) = 0x00 | MSK(unused);                             }, IfRST());
  return IfRST() ? !(context_.is_rst_signaled = false) : false;
}

bool RP2A03::IRQ() noexcept {
  Stage([this] { Push(REG_HI(pc));                                                      }, IfIRQ() && IfNotIRQDisable());
  Stage([this] { Push(REG_LO(pc));                                                      }, IfIRQ() && IfNotIRQDisable());
  Stage([this] { REG(p) |= MSK(unused) | MSK(irq_disable); REG(p) &= ~MSK(brk_command); }, IfIRQ() && IfNotIRQDisable());
  Stage([this] { Push(REG(p));                                                          }, IfIRQ() && IfNotIRQDisable());
  Stage([this] { AddrLo(Read(RP2A03::kBRKAddress));                                     }, IfIRQ() && IfNotIRQDisable());
  Stage([this] { AddrHi(Read(RP2A03::kBRKAddress + 1)); REG(pc) = Addr();               }, IfIRQ() && IfNotIRQDisable());
  return IfIRQ() ? !(context_.is_irq_signaled = false) : false;
}

bool RP2A03::NMI() noexcept {
  Stage([this] { Read(REG(pc));                                           }, IfNMI());
  Stage([this] { Push(REG_HI(pc));                                        }, IfNMI());
  Stage([this] { Push(REG_LO(pc));                                        }, IfNMI());
  Stage([this] { Push(REG(p));                                            }, IfNMI());
  Stage([this] { AddrLo(Read(RP2A03::kNMIAddress));                       }, IfNMI());
  Stage([this] { AddrHi(Read(RP2A03::kNMIAddress + 1)); REG(pc) = Addr(); }, IfNMI());
  return IfNMI() ? !(context_.is_nmi_signaled = false) : false;
}
  
}  // namespace detail
}  // namespace core
}  // namespace nesdev

