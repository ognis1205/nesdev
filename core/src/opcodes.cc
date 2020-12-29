/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <map>
#include <string>
#include "nesdev/core/macros.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

/*
 * NOTE:
 * The following instructions are defined according to these references:
 * [SEE] https://www.masswerk.at/6502/6502_instruction_set.html
 * [SEE] https://undisbeliever.net/snesdev/65816-opcodes.htm
 * [SEE] http://nparker.llx.com/a2/opcodes.html
 */
static const std::map<Byte, Opcode> lookup = {
  {0x00, {Instruction::BRK, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x01, {Instruction::ORA, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0x02, {Instruction::COP, AddressingMode::IMM,  MemoryAccess::READ             }}, // ***65C816***
  {0x03, {Instruction::ORA, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816***
  {0x04, {Instruction::TSB, AddressingMode::DP,   MemoryAccess::READ             }}, // ***65C816***
  {0x05, {Instruction::ORA, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0x06, {Instruction::ASL, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x07, {Instruction::ORA, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816***
  {0x08, {Instruction::PHP, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x09, {Instruction::ORA, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0x0A, {Instruction::ASL, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x0B, {Instruction::PHD, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816***
  {0x0C, {Instruction::TSB, AddressingMode::ABS,  MemoryAccess::READ             }}, // ***65C02-***
  {0x0D, {Instruction::ORA, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x0E, {Instruction::ASL, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x0F, {Instruction::ORA, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816***
  {0x10, {Instruction::BPL, AddressingMode::REL,  MemoryAccess::READ             }},
  {0x11, {Instruction::ORA, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0x12, {Instruction::ORA, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x13, {Instruction::ORA, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816***
  {0x14, {Instruction::TRB, AddressingMode::DP,   MemoryAccess::READ             }}, // ***65C02-***
  {0x15, {Instruction::ORA, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0x16, {Instruction::ASL, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x17, {Instruction::ORA, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816***
  {0x18, {Instruction::CLC, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x19, {Instruction::ORA, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0x1A, {Instruction::INC, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}}, // ***65C02-***
  {0x1B, {Instruction::TCS, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816***
  {0x1C, {Instruction::TRB, AddressingMode::ABS,  MemoryAccess::READ             }}, // ***65C02-***
  {0x1D, {Instruction::ORA, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0x1E, {Instruction::ASL, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x1F, {Instruction::ORA, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816***
  {0x20, {Instruction::JSR, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x21, {Instruction::AND, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0x22, {Instruction::JSR, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816***
  {0x23, {Instruction::AND, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0x24, {Instruction::BIT, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0x25, {Instruction::AND, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0x26, {Instruction::ROL, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x27, {Instruction::AND, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0x28, {Instruction::PLP, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x29, {Instruction::AND, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0x2A, {Instruction::ROL, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x2B, {Instruction::PLD, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x2C, {Instruction::BIT, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x2D, {Instruction::AND, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x2E, {Instruction::ROL, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x2F, {Instruction::AND, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0x30, {Instruction::BMI, AddressingMode::REL,  MemoryAccess::READ             }},
  {0x31, {Instruction::AND, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0x32, {Instruction::AND, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x33, {Instruction::AND, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0x34, {Instruction::BIT, AddressingMode::DPX,  MemoryAccess::READ             }}, // ***65C02-***
  {0x35, {Instruction::AND, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0x36, {Instruction::ROL, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x37, {Instruction::AND, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0x38, {Instruction::SEC, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x39, {Instruction::AND, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0x3A, {Instruction::DEC, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}}, // ***65C02-***
  {0x3B, {Instruction::TSC, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x3C, {Instruction::BIT, AddressingMode::ABX,  MemoryAccess::READ             }}, // ***65C02-***
  {0x3D, {Instruction::AND, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0x3E, {Instruction::ROL, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x3F, {Instruction::AND, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
  {0x40, {Instruction::RTI, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x41, {Instruction::EOR, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0x42, {Instruction::WDM, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x43, {Instruction::EOR, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0x44, {Instruction::MVP, AddressingMode::BM,   MemoryAccess::READ             }}, // ***65C02-***
  {0x45, {Instruction::EOR, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0x46, {Instruction::LSR, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x47, {Instruction::EOR, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0x48, {Instruction::PHA, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x49, {Instruction::EOR, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0x4A, {Instruction::LSR, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x4B, {Instruction::PHK, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x4C, {Instruction::JMP, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x4D, {Instruction::EOR, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x4E, {Instruction::LSR, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x4F, {Instruction::EOR, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0x50, {Instruction::BVC, AddressingMode::REL,  MemoryAccess::READ             }},
  {0x51, {Instruction::EOR, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0x52, {Instruction::EOR, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x53, {Instruction::EOR, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0x54, {Instruction::MVN, AddressingMode::BM,   MemoryAccess::READ             }}, // ***65C816**
  {0x55, {Instruction::EOR, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0x56, {Instruction::LSR, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x57, {Instruction::EOR, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0x58, {Instruction::CLI, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x59, {Instruction::EOR, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0x5A, {Instruction::PHY, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x5B, {Instruction::TCD, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x5C, {Instruction::JMP, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0x5D, {Instruction::EOR, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0x5E, {Instruction::LSR, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x5F, {Instruction::EOR, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
  {0x60, {Instruction::RTS, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x61, {Instruction::ADC, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0x62, {Instruction::PER, AddressingMode::RELL, MemoryAccess::READ             }}, // ***65C816**
  {0x63, {Instruction::ADC, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0x64, {Instruction::STZ, AddressingMode::DP,   MemoryAccess::WRITE            }}, // ***65C02-***
  {0x65, {Instruction::ADC, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0x66, {Instruction::ROR, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x67, {Instruction::ADC, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0x68, {Instruction::PLA, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x69, {Instruction::ADC, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0x6A, {Instruction::ROR, AddressingMode::ACC,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x6B, {Instruction::RTL, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x6C, {Instruction::JMP, AddressingMode::IND,  MemoryAccess::READ             }},
  {0x6D, {Instruction::ADC, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0x6E, {Instruction::ROR, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x6F, {Instruction::ADC, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0x70, {Instruction::BVS, AddressingMode::REL,  MemoryAccess::READ             }},
  {0x71, {Instruction::ADC, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0x72, {Instruction::ADC, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x73, {Instruction::ADC, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0x74, {Instruction::STZ, AddressingMode::DPX,  MemoryAccess::WRITE            }}, // ***65C02-***
  {0x75, {Instruction::ADC, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0x76, {Instruction::ROR, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x77, {Instruction::ADC, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0x78, {Instruction::SEI, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x79, {Instruction::ADC, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0x7A, {Instruction::PLY, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C02-***
  {0x7B, {Instruction::TDC, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x7C, {Instruction::JMP, AddressingMode::IAL,  MemoryAccess::READ             }}, // ***65C02-***
  {0x7D, {Instruction::ADC, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0x7E, {Instruction::ROR, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x7F, {Instruction::ADC, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
  {0x80, {Instruction::BRA, AddressingMode::REL,  MemoryAccess::READ             }}, // ***65C02-***
  {0x81, {Instruction::STA, AddressingMode::IZX,  MemoryAccess::WRITE            }},
  {0x82, {Instruction::BRL, AddressingMode::RELL, MemoryAccess::READ             }}, // ***65C816**
  {0x83, {Instruction::STA, AddressingMode::SR,   MemoryAccess::WRITE            }}, // ***65C816**
  {0x84, {Instruction::STY, AddressingMode::ZP0,  MemoryAccess::WRITE            }},
  {0x85, {Instruction::STA, AddressingMode::ZP0,  MemoryAccess::WRITE            }},
  {0x86, {Instruction::STX, AddressingMode::ZP0,  MemoryAccess::WRITE            }},
  {0x87, {Instruction::STA, AddressingMode::IDL,  MemoryAccess::WRITE            }}, // ***65C816**
  {0x88, {Instruction::DEY, AddressingMode::IMP,  MemoryAccess::READ_MODIFY_WRITE}},
  {0x89, {Instruction::BIT, AddressingMode::IMM,  MemoryAccess::READ             }}, // ***65C02-***
  {0x8A, {Instruction::TXA, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x8B, {Instruction::PHB, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x8C, {Instruction::STY, AddressingMode::ABS,  MemoryAccess::WRITE            }},
  {0x8D, {Instruction::STA, AddressingMode::ABS,  MemoryAccess::WRITE            }},
  {0x8E, {Instruction::STX, AddressingMode::ABS,  MemoryAccess::WRITE            }},
  {0x8F, {Instruction::STA, AddressingMode::ABL,  MemoryAccess::WRITE            }}, // ***65C816**
  {0x90, {Instruction::BCC, AddressingMode::REL,  MemoryAccess::READ             }},
  {0x91, {Instruction::STA, AddressingMode::IZY,  MemoryAccess::WRITE            }},
  {0x92, {Instruction::STA, AddressingMode::IDP,  MemoryAccess::WRITE            }}, // ***65C02-***
  {0x93, {Instruction::STA, AddressingMode::ISY,  MemoryAccess::WRITE            }}, // ***65C816**
  {0x94, {Instruction::STY, AddressingMode::ZPX,  MemoryAccess::WRITE            }},
  {0x95, {Instruction::STA, AddressingMode::ZPX,  MemoryAccess::WRITE            }},
  {0x96, {Instruction::STX, AddressingMode::ZPY,  MemoryAccess::WRITE            }},
  {0x97, {Instruction::STA, AddressingMode::IDLY, MemoryAccess::WRITE            }}, // ***65C816**
  {0x98, {Instruction::TYA, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x99, {Instruction::STA, AddressingMode::ABY,  MemoryAccess::WRITE            }},
  {0x9A, {Instruction::TXS, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0x9B, {Instruction::TXY, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0x9C, {Instruction::STZ, AddressingMode::ABS,  MemoryAccess::WRITE            }}, // ***65C02-***
  {0x9D, {Instruction::STA, AddressingMode::ABX,  MemoryAccess::WRITE            }},
  {0x9E, {Instruction::STZ, AddressingMode::ABX,  MemoryAccess::WRITE            }}, // ***65C02-***
  {0x9F, {Instruction::STA, AddressingMode::ALX,  MemoryAccess::WRITE            }}, // ***65C816**
  {0xA0, {Instruction::LDY, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xA1, {Instruction::LDA, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0xA2, {Instruction::LDX, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xA3, {Instruction::LDA, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0xA4, {Instruction::LDY, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xA5, {Instruction::LDA, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xA6, {Instruction::LDX, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xA7, {Instruction::LDA, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0xA8, {Instruction::TAY, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xA9, {Instruction::LDA, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xAA, {Instruction::TAX, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xAB, {Instruction::PLB, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xAC, {Instruction::LDY, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xAD, {Instruction::LDA, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xAE, {Instruction::LDX, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xAF, {Instruction::LDA, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0xB0, {Instruction::BCS, AddressingMode::REL,  MemoryAccess::READ             }},
  {0xB1, {Instruction::LDA, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0xB2, {Instruction::LDA, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0xB3, {Instruction::LDA, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0xB4, {Instruction::LDY, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0xB5, {Instruction::LDA, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0xB6, {Instruction::LDX, AddressingMode::ZPY,  MemoryAccess::READ             }},
  {0xB7, {Instruction::LDA, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0xB8, {Instruction::CLV, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xB9, {Instruction::LDA, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0xBA, {Instruction::TSX, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xBB, {Instruction::TYX, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xBC, {Instruction::LDY, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0xBD, {Instruction::LDA, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0xBE, {Instruction::LDX, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0xBF, {Instruction::LDA, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
  {0xC0, {Instruction::CPY, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xC1, {Instruction::CMP, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0xC2, {Instruction::REP, AddressingMode::IMM,  MemoryAccess::READ             }}, // ***65C816**
  {0xC3, {Instruction::CMP, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0xC4, {Instruction::CPY, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xC5, {Instruction::CMP, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xC6, {Instruction::DEC, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xC7, {Instruction::CMP, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0xC8, {Instruction::INY, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xC9, {Instruction::CMP, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xCA, {Instruction::DEX, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xCB, {Instruction::WAI, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xCC, {Instruction::CPY, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xCD, {Instruction::CMP, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xCE, {Instruction::DEC, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xCF, {Instruction::CMP, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0xD0, {Instruction::BNE, AddressingMode::REL,  MemoryAccess::READ             }},
  {0xD1, {Instruction::CMP, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0xD2, {Instruction::CMP, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0xD3, {Instruction::CMP, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0xD4, {Instruction::PEI, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C816**
  {0xD5, {Instruction::CMP, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0xD6, {Instruction::DEC, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xD7, {Instruction::CMP, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0xD8, {Instruction::CLD, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xD9, {Instruction::CMP, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0xDA, {Instruction::PHX, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C02-***
  {0xDB, {Instruction::STP, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xDC, {Instruction::JML, AddressingMode::IAX,  MemoryAccess::READ             }}, // ***65C816**
  {0xDD, {Instruction::CMP, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0xDE, {Instruction::DEC, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xDF, {Instruction::CMP, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
  {0xE0, {Instruction::CPX, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xE1, {Instruction::SBC, AddressingMode::IZX,  MemoryAccess::READ             }},
  {0xE2, {Instruction::SEP, AddressingMode::IMM,  MemoryAccess::READ             }}, // ***65C816**
  {0xE3, {Instruction::SBC, AddressingMode::SR,   MemoryAccess::READ             }}, // ***65C816**
  {0xE4, {Instruction::CPX, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xE5, {Instruction::SBC, AddressingMode::ZP0,  MemoryAccess::READ             }},
  {0xE6, {Instruction::INC, AddressingMode::ZP0,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xE7, {Instruction::SBC, AddressingMode::IDL,  MemoryAccess::READ             }}, // ***65C816**
  {0xE8, {Instruction::INX, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xE9, {Instruction::SBC, AddressingMode::IMM,  MemoryAccess::READ             }},
  {0xEA, {Instruction::NOP, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xEB, {Instruction::XBA, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xEC, {Instruction::CPX, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xED, {Instruction::SBC, AddressingMode::ABS,  MemoryAccess::READ             }},
  {0xEE, {Instruction::INC, AddressingMode::ABS,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xEF, {Instruction::SBC, AddressingMode::ABL,  MemoryAccess::READ             }}, // ***65C816**
  {0xF0, {Instruction::BEQ, AddressingMode::REL,  MemoryAccess::READ             }},
  {0xF1, {Instruction::SBC, AddressingMode::IZY,  MemoryAccess::READ             }},
  {0xF2, {Instruction::SBC, AddressingMode::IDP,  MemoryAccess::READ             }}, // ***65C02-***
  {0xF3, {Instruction::SBC, AddressingMode::ISY,  MemoryAccess::READ             }}, // ***65C816**
  {0xF4, {Instruction::PEA, AddressingMode::ABS,  MemoryAccess::READ             }}, // ***65C816**
  {0xF5, {Instruction::SBC, AddressingMode::ZPX,  MemoryAccess::READ             }},
  {0xF6, {Instruction::INC, AddressingMode::ZPX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xF7, {Instruction::SBC, AddressingMode::IDLY, MemoryAccess::READ             }}, // ***65C816**
  {0xF8, {Instruction::SED, AddressingMode::IMP,  MemoryAccess::READ             }},
  {0xF9, {Instruction::SBC, AddressingMode::ABY,  MemoryAccess::READ             }},
  {0xFA, {Instruction::PLX, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C02-***
  {0xFB, {Instruction::XCE, AddressingMode::IMP,  MemoryAccess::READ             }}, // ***65C816**
  {0xFC, {Instruction::JSR, AddressingMode::IAX,  MemoryAccess::READ             }}, // ***65C816**
  {0xFD, {Instruction::SBC, AddressingMode::ABX,  MemoryAccess::READ             }},
  {0xFE, {Instruction::INC, AddressingMode::ABX,  MemoryAccess::READ_MODIFY_WRITE}},
  {0xFF, {Instruction::SBC, AddressingMode::ALX,  MemoryAccess::READ             }}, // ***65C816**
};

Opcode Decode(const Byte& byte) NESDEV_CORE_NOEXCEPT {
  return lookup.at(byte);
}

static std::string ToString(const Instruction& instruction) NESDEV_CORE_NOEXCEPT {
  switch (instruction) {
  case ADC: return "ADC";
  case AND: return "AND";
  case ASL: return "ASL";
  case BCC: return "BCC";
  case BCS: return "BCS";
  case BEQ: return "BEQ";
  case BNE: return "BNE";
  case BMI: return "BMI";
  case BPL: return "BPL";
  case BVC: return "BVC";
  case BVS: return "BVS";
  case BRA: return "BRA";
  case BRL: return "BRL";
  case BIT: return "BIT";
  case BRK: return "BRK";
  case COP: return "COP";
  case CLC: return "CLC";
  case CLI: return "CLI";
  case CLD: return "CLD";
  case CLV: return "CLV";
  case CMP: return "CMP";
  case CPX: return "CPX";
  case CPY: return "CPY";
  case DEC: return "DEC";
  case DEX: return "DEX";
  case DEY: return "DEY";
  case EOR: return "EOR";
  case INC: return "INC";
  case INX: return "INX";
  case INY: return "INY";
  case JMP: return "JMP";
  case JML: return "JML";
  case JSR: return "JSR";
  case JSL: return "JSL";
  case LDA: return "LDA";
  case LDX: return "LDX";
  case LDY: return "LDY";
  case LSR: return "LSR";
  case MVN: return "MVN";
  case MVP: return "MVP";
  case NOP: return "NOP";
  case ORA: return "ORA";
  case PEA: return "PEA";
  case PEI: return "PEI";
  case PER: return "PER";
  case PHA: return "PHA";
  case PHB: return "PHB";
  case PHD: return "PHD";
  case PHK: return "PHK";
  case PHP: return "PHP";
  case PHX: return "PHX";
  case PHY: return "PHY";
  case PLA: return "PLA";
  case PLB: return "PLB";
  case PLD: return "PLD";
  case PLP: return "PLP";
  case PLX: return "PLX";
  case PLY: return "PLY";
  case REP: return "REP";
  case ROL: return "ROL";
  case ROR: return "ROR";
  case RTI: return "RTI";
  case RTS: return "RTS";
  case RTL: return "RTL";
  case SBC: return "SBC";
  case SEC: return "SEC";
  case SEI: return "SEI";
  case SED: return "SEd";
  case SEP: return "SEP";
  case STP: return "STP";
  case STA: return "STA";
  case STX: return "STX";
  case STY: return "STY";
  case STZ: return "STZ";
  case TAX: return "TAX";
  case TAY: return "TAY";
  case TCD: return "TCD";
  case TCS: return "TCS";
  case TDC: return "TDC";
  case TSC: return "TSC";
  case TSX: return "TSX";
  case TXA: return "TXA";
  case TXS: return "TXS";
  case TXY: return "TXY";
  case TYA: return "TYA";
  case TYX: return "TYX";
  case TRB: return "TRB";
  case TSB: return "TSB";
  case WAI: return "WAI";
  case WDM: return "WDM";
  case XBA: return "XBA";
  case XCE: return "XCE";
  default:  return "UNKNOWN";
  }
}

static std::string ToString(const AddressingMode& addressing_mode) NESDEV_CORE_NOEXCEPT {
  switch (addressing_mode) {
  case ABS:  return "ABS";
  case ABX:  return "ABX";
  case ABY:  return "ABY";
  case ACC:  return "ACC";
  case IMM:  return "IMM";
  case IMP:  return "IMP";
  case IND:  return "IND";
  case IZX:  return "IZX";
  case IZY:  return "IZY";
  case REL:  return "REL";
  case ZP0:  return "ZP0";
  case ZPX:  return "ZPX";
  case ZPY:  return "ZPY";
  case SR:   return "SR";
  case DP:   return "DP";
  case DPX:  return "DPX";
  case DPY:  return "DPY";
  case IDP:  return "IDP";
  case IDX:  return "IDX";
  case IDY:  return "IDY";
  case IDL:  return "IDL";
  case IDLY: return "IDLY";
  case ISY:  return "ISY";
  case ABL:  return "ABL";
  case ALX:  return "ALX";
  case IAX:  return "IAX";
  case IAL:  return "IAL";
  case RELL: return "RELL";
  case BM:   return "BM";
  default:   return "UNKNOWN";
  }
}

std::string ToString(const Byte& byte) NESDEV_CORE_NOEXCEPT {
  return ToString(Decode(byte).instruction) + ", " + ToString(Decode(byte).addressing_mode);
}

}  // namespace core
}  // namespace nesdev
