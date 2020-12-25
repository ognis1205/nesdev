/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <map>
#include "nes/core/opcodes.h"
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
static const std::map<Byte, const Opcode> lookup = {
  {0x00, {Instruction::BRK, AddressingMode::IMP }},
  {0x01, {Instruction::ORA, AddressingMode::IZX }},
  {0x02, {Instruction::COP, AddressingMode::IMM }}, // ***65C816***
  {0x03, {Instruction::ORA, AddressingMode::SR  }}, // ***65C816***
  {0x04, {Instruction::TSB, AddressingMode::DP  }}, // ***65C816***
  {0x05, {Instruction::ORA, AddressingMode::ZP0 }},
  {0x06, {Instruction::ASL, AddressingMode::ZP0 }},
  {0x07, {Instruction::ORA, AddressingMode::IDL }}, // ***65C816***
  {0x08, {Instruction::PHP, AddressingMode::IMP }},
  {0x09, {Instruction::ORA, AddressingMode::IMM }},
  {0x0A, {Instruction::ASL, AddressingMode::ACC }},
  {0x0B, {Instruction::PHD, AddressingMode::IMP }}, // ***65C816***
  {0x0C, {Instruction::TSB, AddressingMode::ABS }}, // ***65C02-***
  {0x0D, {Instruction::ORA, AddressingMode::ABS }},
  {0x0E, {Instruction::ASL, AddressingMode::ABS }},
  {0x0F, {Instruction::ORA, AddressingMode::ABL }},
  {0x10, {Instruction::BPL, AddressingMode::REL }},
  {0x11, {Instruction::ORA, AddressingMode::IZY }},
  {0x12, {Instruction::ORA, AddressingMode::IDP }}, // ***65C02-***
  {0x13, {Instruction::ORA, AddressingMode::ISY }}, // ***65C816***
  {0x14, {Instruction::TRB, AddressingMode::DP  }}, // ***65C02-***
  {0x15, {Instruction::ORA, AddressingMode::ZPX }},
  {0x16, {Instruction::ASL, AddressingMode::ZPX }},
  {0x17, {Instruction::ORA, AddressingMode::IDLY}}, // ***65C816***
  {0x18, {Instruction::CLC, AddressingMode::IMP }},
  {0x19, {Instruction::ORA, AddressingMode::ABY }},
  {0x1A, {Instruction::INC, AddressingMode::ACC }}, // ***65C02-***
  {0x1B, {Instruction::TCS, AddressingMode::IMP }}, // ***65C816***
  {0x1C, {Instruction::TRB, AddressingMode::ABS }}, // ***65C02-***
  {0x1D, {Instruction::ORA, AddressingMode::ABX }},
  {0x1E, {Instruction::ASL, AddressingMode::ABX }},
  {0x1F, {Instruction::ORA, AddressingMode::ALX }}, // ***65C816***
  {0x20, {Instruction::JSR, AddressingMode::ABS }},
  {0x21, {Instruction::AND, AddressingMode::IZX }},
  {0x22, {Instruction::JSL, AddressingMode::ABL }}, // ***65C816***
  {0x23, {Instruction::AND, AddressingMode::SR  }}, // ***65C816**
  {0x24, {Instruction::BIT, AddressingMode::ZP0 }},
  {0x25, {Instruction::AND, AddressingMode::ZP0 }},
  {0x26, {Instruction::ROL, AddressingMode::ZP0 }},
  {0x27, {Instruction::AND, AddressingMode::IDL }}, // ***65C816**
  {0x28, {Instruction::PLP, AddressingMode::IMP }},
  {0x29, {Instruction::AND, AddressingMode::IMM }},
  {0x2A, {Instruction::ROL, AddressingMode::ACC }},
  {0x2B, {Instruction::PLD, AddressingMode::IMP }}, // ***65C816**
  {0x2C, {Instruction::BIT, AddressingMode::ABS }},
  {0x2D, {Instruction::AND, AddressingMode::ABS }},
  {0x2E, {Instruction::ROL, AddressingMode::ABS }},
  {0x2F, {Instruction::AND, AddressingMode::ABL }}, // ***65C816**
  {0x30, {Instruction::BMI, AddressingMode::REL }},
  {0x31, {Instruction::AND, AddressingMode::IZY }},
  {0x32, {Instruction::AND, AddressingMode::IDP }}, // ***65C02-***
  {0x33, {Instruction::AND, AddressingMode::ISY }}, // ***65C816**
  {0x34, {Instruction::BIT, AddressingMode::DPX }}, // ***65C02-***
  {0x35, {Instruction::AND, AddressingMode::ZPX }},
  {0x36, {Instruction::ROL, AddressingMode::ZPX }},
  {0x37, {Instruction::AND, AddressingMode::IDLY}}, // ***65C816**
  {0x38, {Instruction::SEC, AddressingMode::IMP }},
  {0x39, {Instruction::AND, AddressingMode::ABY }},
  {0x3A, {Instruction::DEC, AddressingMode::ACC }}, // ***65C02-***
  {0x3B, {Instruction::TSC, AddressingMode::IMP }}, // ***65C816**
  {0x3C, {Instruction::BIT, AddressingMode::ABX }}, // ***65C02-***
  {0x3D, {Instruction::AND, AddressingMode::ABX }},
  {0x3E, {Instruction::ROL, AddressingMode::ABX }},
  {0x3F, {Instruction::AND, AddressingMode::ALX }}, // ***65C816**
  {0x40, {Instruction::RTI, AddressingMode::IMP }},
  {0x41, {Instruction::EOR, AddressingMode::IZX }},
  {0x42, {Instruction::WDM, AddressingMode::IMP }}, // ***65C816**
  {0x43, {Instruction::EOR, AddressingMode::SR  }}, // ***65C816**
  {0x44, {Instruction::MVP, AddressingMode::BM  }}, // ***65C02-***
  {0x45, {Instruction::EOR, AddressingMode::ZP0 }},
  {0x46, {Instruction::LSR, AddressingMode::ZP0 }},
  {0x47, {Instruction::EOR, AddressingMode::IDL }}, // ***65C816**
  {0x48, {Instruction::PHA, AddressingMode::IMP }},
  {0x49, {Instruction::EOR, AddressingMode::IMM }},
  {0x4A, {Instruction::LSR, AddressingMode::ACC }},
  {0x4B, {Instruction::PHK, AddressingMode::IMP }}, // ***65C816**
  {0x4C, {Instruction::JMP, AddressingMode::ABS }},
  {0x4D, {Instruction::EOR, AddressingMode::ABS }},
  {0x4E, {Instruction::LSR, AddressingMode::ABS }},
  {0x4F, {Instruction::EOR, AddressingMode::ABL }}, // ***65C816**
  {0x50, {Instruction::BVC, AddressingMode::REL }},
  {0x51, {Instruction::EOR, AddressingMode::IZY }},
  {0x52, {Instruction::EOR, AddressingMode::IDP }}, // ***65C02-***
  {0x53, {Instruction::EOR, AddressingMode::ISY }}, // ***65C816**
  {0x54, {Instruction::MVN, AddressingMode::BM  }}, // ***65C816**
  {0x55, {Instruction::EOR, AddressingMode::ZPX }},
  {0x56, {Instruction::LSR, AddressingMode::ZPX }},
  {0x57, {Instruction::EOR, AddressingMode::IDLY}}, // ***65C816**
  {0x58, {Instruction::CLI, AddressingMode::IMP }},
  {0x59, {Instruction::EOR, AddressingMode::ABY }},
  {0x5A, {Instruction::PHY, AddressingMode::IMP }}, // ***65C02-***
  {0x5B, {Instruction::TCD, AddressingMode::IMP }}, // ***65C816**
  {0x5C, {Instruction::JMP, AddressingMode::ABL }}, // ***65C816**
  {0x5D, {Instruction::EOR, AddressingMode::ABX }},
  {0x5E, {Instruction::LSR, AddressingMode::ABX }},
  {0x5F, {Instruction::EOR, AddressingMode::ALX }}, // ***65C816**
  {0x60, {Instruction::RTS, AddressingMode::IMP }},
  {0x61, {Instruction::ADC, AddressingMode::IZX }},
  {0x62, {Instruction::PER, AddressingMode::RELL}}, // ***65C816**
  {0x63, {Instruction::ADC, AddressingMode::SR  }}, // ***65C816**
  {0x64, {Instruction::STZ, AddressingMode::DP  }}, // ***65C02-***
  {0x65, {Instruction::ADC, AddressingMode::ZP0 }},
  {0x66, {Instruction::ROR, AddressingMode::ZP0 }},
  {0x67, {Instruction::ADC, AddressingMode::IDL }}, // ***65C816**
  {0x68, {Instruction::PLA, AddressingMode::IMP }},
  {0x69, {Instruction::ADC, AddressingMode::IMM }},
  {0x6A, {Instruction::ROR, AddressingMode::ACC }},
  {0x6B, {Instruction::RTL, AddressingMode::IMP }}, // ***65C816**
  {0x6C, {Instruction::JMP, AddressingMode::IND }},
  {0x6D, {Instruction::ADC, AddressingMode::ABS }},
  {0x6E, {Instruction::ROR, AddressingMode::ABS }},
  {0x6F, {Instruction::ADC, AddressingMode::ABL }}, // ***65C816**
  {0x70, {Instruction::BVS, AddressingMode::REL }},
  {0x71, {Instruction::ADC, AddressingMode::IZY }},
  {0x72, {Instruction::ADC, AddressingMode::IDP }}, // ***65C02-***
  {0x73, {Instruction::ADC, AddressingMode::ISY }}, // ***65C816**
  {0x74, {Instruction::STZ, AddressingMode::DPX }}, // ***65C02-***
  {0x75, {Instruction::ADC, AddressingMode::ZPX }},
  {0x76, {Instruction::ROR, AddressingMode::ZPX }},
  {0x77, {Instruction::ADC, AddressingMode::IDLY}}, // ***65C816**
  {0x78, {Instruction::SEI, AddressingMode::IMP }},
  {0x79, {Instruction::ADC, AddressingMode::ABY }},
  {0x7A, {Instruction::PLY, AddressingMode::IMP }}, // ***65C02-***
  {0x7B, {Instruction::TDC, AddressingMode::IMP }}, // ***65C816**
  {0x7C, {Instruction::JMP, AddressingMode::IAL }}, // ***65C02-***
  {0x7D, {Instruction::ADC, AddressingMode::ABX }},
  {0x7E, {Instruction::ROR, AddressingMode::ABX }},
  {0x7F, {Instruction::ADC, AddressingMode::ALX }}, // ***65C816**
  {0x80, {Instruction::BRA, AddressingMode::REL }}, // ***65C02-***
  {0x81, {Instruction::STA, AddressingMode::IZX }},
  {0x82, {Instruction::BRL, AddressingMode::REL }}, // ***65C816**
  {0x83, {Instruction::STA, AddressingMode::SR  }}, // ***65C816**
  {0x84, {Instruction::STY, AddressingMode::ZP0 }},
  {0x85, {Instruction::STA, AddressingMode::ZP0 }},
  {0x86, {Instruction::STX, AddressingMode::ZP0 }},
  {0x87, {Instruction::STA, AddressingMode::IDL }}, // ***65C816**
  {0x88, {Instruction::DEY, AddressingMode::IMP }},
  {0x89, {Instruction::BIT, AddressingMode::IMM }}, // ***65C02-***
  {0x8A, {Instruction::TXA, AddressingMode::IMP }},
  {0x8B, {Instruction::PHB, AddressingMode::IMP }}, // ***65C816**
  {0x8C, {Instruction::STY, AddressingMode::ABS }},
  {0x8D, {Instruction::STA, AddressingMode::ABS }},
  {0x8E, {Instruction::STX, AddressingMode::ABS }},
  {0x8F, {Instruction::STA, AddressingMode::ABL }}, // ***65C816**
  {0x90, {Instruction::BCC, AddressingMode::REL }},
  {0x91, {Instruction::STA, AddressingMode::IZY }},
  {0x92, {Instruction::STA, AddressingMode::IDP }}, // ***65C02-***
  {0x93, {Instruction::STA, AddressingMode::ISY }}, // ***65C816**
  {0x94, {Instruction::STY, AddressingMode::ZPX }},
  {0x95, {Instruction::STA, AddressingMode::ZPX }},
  {0x96, {Instruction::STX, AddressingMode::ZPY }},
  {0x97, {Instruction::STA, AddressingMode::IDLY}}, // ***65C816**
  {0x98, {Instruction::TYA, AddressingMode::IMP }},
  {0x99, {Instruction::STA, AddressingMode::ABY }},
  {0x9A, {Instruction::TXS, AddressingMode::IMP }},
  {0x9B, {Instruction::TXY, AddressingMode::IMP }}, // ***65C816**
  {0x9C, {Instruction::STZ, AddressingMode::ABS }}, // ***65C02-***
  {0x9D, {Instruction::STA, AddressingMode::ABX }},
  {0x9E, {Instruction::STZ, AddressingMode::ABX }}, // ***65C02-***
  {0x9F, {Instruction::STA, AddressingMode::ALX }}, // ***65C816**
  {0xA0, {Instruction::LDY, AddressingMode::IMM }},
  {0xA1, {Instruction::LDA, AddressingMode::IZX }},
  {0xA2, {Instruction::LDX, AddressingMode::IMM }},
  {0xA3, {Instruction::LDA, AddressingMode::SR  }}, // ***65C816**
  {0xA4, {Instruction::LDY, AddressingMode::ZP0 }},
  {0xA5, {Instruction::LDA, AddressingMode::ZP0 }},
  {0xA6, {Instruction::LDX, AddressingMode::ZP0 }},
  {0xA7, {Instruction::LDA, AddressingMode::IDL }}, // ***65C816**
  {0xA8, {Instruction::TAY, AddressingMode::IMP }},
  {0xA9, {Instruction::LDA, AddressingMode::IMM }},
  {0xAA, {Instruction::TAX, AddressingMode::IMP }},
  {0xAB, {Instruction::PLB, AddressingMode::IMP }}, // ***65C816**
  {0xAC, {Instruction::LDY, AddressingMode::ABS }},
  {0xAD, {Instruction::LDA, AddressingMode::ABS }},
  {0xAE, {Instruction::LDX, AddressingMode::ABS }},
  {0xAF, {Instruction::LDA, AddressingMode::ABL }}, // ***65C816**
  {0xB0, {Instruction::BCS, AddressingMode::REL }},
  {0xB1, {Instruction::LDA, AddressingMode::IZX }},
  {0xB2, {Instruction::LDA, AddressingMode::IDP }}, // ***65C02-***
  {0xB3, {Instruction::LDA, AddressingMode::ISY }}, // ***65C816**
  {0xB4, {Instruction::LDY, AddressingMode::ZPX }},
  {0xB5, {Instruction::LDA, AddressingMode::ZPX }},
  {0xB6, {Instruction::LDX, AddressingMode::ZPY }},
  {0xB7, {Instruction::LDA, AddressingMode::IDLY}}, // ***65C816**
  {0xB8, {Instruction::CLV, AddressingMode::IMP }},
  {0xB9, {Instruction::LDA, AddressingMode::ABY }},
  {0xBA, {Instruction::TSX, AddressingMode::IMP }},
  {0xBB, {Instruction::TYX, AddressingMode::IMP }}, // ***65C816**
  {0xBC, {Instruction::LDY, AddressingMode::ABX }},
  {0xBD, {Instruction::LDA, AddressingMode::ABX }},
  {0xBE, {Instruction::LDX, AddressingMode::ABY }},
  {0xBF, {Instruction::LDA, AddressingMode::ALX }}, // ***65C816**
  {0xC0, {Instruction::CPY, AddressingMode::IMM }},
  {0xC1, {Instruction::CMP, AddressingMode::IZX }},
  {0xC2, {Instruction::REP, AddressingMode::IMM }}, // ***65C816**
  {0xC3, {Instruction::CMP, AddressingMode::SR  }}, // ***65C816**
  {0xC4, {Instruction::CPY, AddressingMode::ZP0 }},
  {0xC5, {Instruction::CMP, AddressingMode::ZP0 }},
  {0xC6, {Instruction::DEC, AddressingMode::ZP0 }},
  {0xC7, {Instruction::CMP, AddressingMode::IDL }}, // ***65C816**
  {0xC8, {Instruction::INY, AddressingMode::IMP }},
  {0xC9, {Instruction::CMP, AddressingMode::IMM }},
  {0xCA, {Instruction::DEX, AddressingMode::IMP }},
  {0xCB, {Instruction::WAI, AddressingMode::IMP }}, // ***65C816**
  {0xCC, {Instruction::CPY, AddressingMode::ABS }},
  {0xCD, {Instruction::CMP, AddressingMode::ABS }},
  {0xCE, {Instruction::DEC, AddressingMode::ABS }},
  {0xCF, {Instruction::CMP, AddressingMode::ABL }}, // ***65C816**
  {0xD0, {Instruction::BNE, AddressingMode::REL }},
  {0xD1, {Instruction::CMP, AddressingMode::IZX }},
  {0xD2, {Instruction::CMP, AddressingMode::IDP }}, // ***65C02-***
  {0xD3, {Instruction::CMP, AddressingMode::ISY }}, // ***65C816**
  {0xD4, {Instruction::PEI, AddressingMode::DP  }}, // ***65C816**
  {0xD5, {Instruction::CMP, AddressingMode::ZPX }},
  {0xD6, {Instruction::DEC, AddressingMode::ZPX }},
  {0xD7, {Instruction::CMP, AddressingMode::IDLY}}, // ***65C816**
  {0xD8, {Instruction::CLD, AddressingMode::IMP }},
  {0xD9, {Instruction::CMP, AddressingMode::ABY }},
  {0xDA, {Instruction::PHX, AddressingMode::IMP }}, // ***65C02-***
  {0xDB, {Instruction::STP, AddressingMode::IMP }}, // ***65C816**
  {0xDC, {Instruction::JML, AddressingMode::IAX }}, // ***65C816**
  {0xDD, {Instruction::CMP, AddressingMode::ABX }},
  {0xDE, {Instruction::DEC, AddressingMode::ABX }},
  {0xDF, {Instruction::CMP, AddressingMode::ALX }}, // ***65C816**
  {0xE0, {Instruction::CPX, AddressingMode::IMM }},
  {0xE1, {Instruction::SBC, AddressingMode::IZX }},
  {0xE2, {Instruction::SEP, AddressingMode::IMM }}, // ***65C816**
  {0xE3, {Instruction::SBC, AddressingMode::SR  }}, // ***65C816**
  {0xE4, {Instruction::CPX, AddressingMode::ZP0 }},
  {0xE5, {Instruction::SBC, AddressingMode::ZP0 }},
  {0xE6, {Instruction::INC, AddressingMode::ZP0 }},
  {0xE7, {Instruction::SBC, AddressingMode::IDL }}, // ***65C816**
  {0xE8, {Instruction::INX, AddressingMode::IMP }},
  {0xE9, {Instruction::SBC, AddressingMode::IMM }},
  {0xEA, {Instruction::NOP, AddressingMode::IMP }},
  {0xEB, {Instruction::XBA, AddressingMode::IMP }}, // ***65C816**
  {0xEC, {Instruction::CPX, AddressingMode::ABX }},
  {0xED, {Instruction::SBC, AddressingMode::ABS }},
  {0xEE, {Instruction::INC, AddressingMode::ABS }},
  {0xEF, {Instruction::SBC, AddressingMode::ABL }}, // ***65C816**
  {0xF0, {Instruction::BEQ, AddressingMode::REL }},
  {0xF1, {Instruction::SBC, AddressingMode::IZX }},
  {0xF2, {Instruction::SBC, AddressingMode::IDP }}, // ***65C02-***
  {0xF3, {Instruction::SBC, AddressingMode::ISY }}, // ***65C816**
  {0xF4, {Instruction::PEA, AddressingMode::ABS }}, // ***65C816**
  {0xF5, {Instruction::SBC, AddressingMode::ZPX }},
  {0xF6, {Instruction::INC, AddressingMode::ZPX }},
  {0xF7, {Instruction::SBC, AddressingMode::IDLY}}, // ***65C816**
  {0xF8, {Instruction::SED, AddressingMode::IMP }},
  {0xF9, {Instruction::SBC, AddressingMode::ABY }},
  {0xFA, {Instruction::PLX, AddressingMode::IMP }}, // ***65C02-***
  {0xFB, {Instruction::XCE, AddressingMode::IMP }}, // ***65C816**
  {0xFC, {Instruction::JSR, AddressingMode::IAL }}, // ***65C816**
  {0xFD, {Instruction::SBC, AddressingMode::ABX }},
  {0xFE, {Instruction::INC, AddressingMode::ABX }},
  {0xFF, {Instruction::SBC, AddressingMode::ALX }},
};

Opcode Decode(const Byte& byte) noexcept {
  return lookup.at(byte);
}

}  // namespace core
}  // namespace nes
