/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "cpu.h"

namespace nes {
namespace cpu {

/*
 * Construct MOS6502 instance:
 * Set/Clear register values and assisstive variables, and generate opcode lookup table.
 */
MOS6502::MOS6502()
    : A{0x00},
      X{0x00},
      Y{0x00},
      S{0x00},
      PC{0x0000},
      P{0x00},
      bus_{nullptr},
      opcode_{0x00},
      operand_{0x00},
      address_{0x0000},
      offset_{0x0000},
      cycles_delayed_{0u},
      clock_count_{0u} {
  lookup_ = {
      {"BRK", &MOS6502::BRK, &MOS6502::IMM, 7},
      {"ORA", &MOS6502::ORA, &MOS6502::IZX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 3},
      {"ORA", &MOS6502::ORA, &MOS6502::ZP0, 3},
      {"ASL", &MOS6502::ASL, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"PHP", &MOS6502::PHP, &MOS6502::IMP, 3},
      {"ORA", &MOS6502::ORA, &MOS6502::IMM, 2},
      {"ASL", &MOS6502::ASL, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"ORA", &MOS6502::ORA, &MOS6502::ABS, 4},
      {"ASL", &MOS6502::ASL, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BPL", &MOS6502::BPL, &MOS6502::REL, 2},
      {"ORA", &MOS6502::ORA, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"ORA", &MOS6502::ORA, &MOS6502::ZPX, 4},
      {"ASL", &MOS6502::ASL, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"CLC", &MOS6502::CLC, &MOS6502::IMP, 2},
      {"ORA", &MOS6502::ORA, &MOS6502::ABY, 4},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"ORA", &MOS6502::ORA, &MOS6502::ABX, 4},
      {"ASL", &MOS6502::ASL, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"JSR", &MOS6502::JSR, &MOS6502::ABS, 6},
      {"AND", &MOS6502::AND, &MOS6502::IZX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"BIT", &MOS6502::BIT, &MOS6502::ZP0, 3},
      {"AND", &MOS6502::AND, &MOS6502::ZP0, 3},
      {"ROL", &MOS6502::ROL, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"PLP", &MOS6502::PLP, &MOS6502::IMP, 4},
      {"AND", &MOS6502::AND, &MOS6502::IMM, 2},
      {"ROL", &MOS6502::ROL, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"BIT", &MOS6502::BIT, &MOS6502::ABS, 4},
      {"AND", &MOS6502::AND, &MOS6502::ABS, 4},
      {"ROL", &MOS6502::ROL, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BMI", &MOS6502::BMI, &MOS6502::REL, 2},
      {"AND", &MOS6502::AND, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"AND", &MOS6502::AND, &MOS6502::ZPX, 4},
      {"ROL", &MOS6502::ROL, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"SEC", &MOS6502::SEC, &MOS6502::IMP, 2},
      {"AND", &MOS6502::AND, &MOS6502::ABY, 4},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"AND", &MOS6502::AND, &MOS6502::ABX, 4},
      {"ROL", &MOS6502::ROL, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"RTI", &MOS6502::RTI, &MOS6502::IMP, 6},
      {"EOR", &MOS6502::EOR, &MOS6502::IZX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 3},
      {"EOR", &MOS6502::EOR, &MOS6502::ZP0, 3},
      {"LSR", &MOS6502::LSR, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"PHA", &MOS6502::PHA, &MOS6502::IMP, 3},
      {"EOR", &MOS6502::EOR, &MOS6502::IMM, 2},
      {"LSR", &MOS6502::LSR, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"JMP", &MOS6502::JMP, &MOS6502::ABS, 3},
      {"EOR", &MOS6502::EOR, &MOS6502::ABS, 4},
      {"LSR", &MOS6502::LSR, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BVC", &MOS6502::BVC, &MOS6502::REL, 2},
      {"EOR", &MOS6502::EOR, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"EOR", &MOS6502::EOR, &MOS6502::ZPX, 4},
      {"LSR", &MOS6502::LSR, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"CLI", &MOS6502::CLI, &MOS6502::IMP, 2},
      {"EOR", &MOS6502::EOR, &MOS6502::ABY, 4},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"EOR", &MOS6502::EOR, &MOS6502::ABX, 4},
      {"LSR", &MOS6502::LSR, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"RTS", &MOS6502::RTS, &MOS6502::IMP, 6},
      {"ADC", &MOS6502::ADC, &MOS6502::IZX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 3},
      {"ADC", &MOS6502::ADC, &MOS6502::ZP0, 3},
      {"ROR", &MOS6502::ROR, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"PLA", &MOS6502::PLA, &MOS6502::IMP, 4},
      {"ADC", &MOS6502::ADC, &MOS6502::IMM, 2},
      {"ROR", &MOS6502::ROR, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"JMP", &MOS6502::JMP, &MOS6502::IND, 5},
      {"ADC", &MOS6502::ADC, &MOS6502::ABS, 4},
      {"ROR", &MOS6502::ROR, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BVS", &MOS6502::BVS, &MOS6502::REL, 2},
      {"ADC", &MOS6502::ADC, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"ADC", &MOS6502::ADC, &MOS6502::ZPX, 4},
      {"ROR", &MOS6502::ROR, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"SEI", &MOS6502::SEI, &MOS6502::IMP, 2},
      {"ADC", &MOS6502::ADC, &MOS6502::ABY, 4},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"ADC", &MOS6502::ADC, &MOS6502::ABX, 4},
      {"ROR", &MOS6502::ROR, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"STA", &MOS6502::STA, &MOS6502::IZX, 6},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"STY", &MOS6502::STY, &MOS6502::ZP0, 3},
      {"STA", &MOS6502::STA, &MOS6502::ZP0, 3},
      {"STX", &MOS6502::STX, &MOS6502::ZP0, 3},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 3},
      {"DEY", &MOS6502::DEY, &MOS6502::IMP, 2},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"TXA", &MOS6502::TXA, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"STY", &MOS6502::STY, &MOS6502::ABS, 4},
      {"STA", &MOS6502::STA, &MOS6502::ABS, 4},
      {"STX", &MOS6502::STX, &MOS6502::ABS, 4},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"BCC", &MOS6502::BCC, &MOS6502::REL, 2},
      {"STA", &MOS6502::STA, &MOS6502::IZY, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"STY", &MOS6502::STY, &MOS6502::ZPX, 4},
      {"STA", &MOS6502::STA, &MOS6502::ZPX, 4},
      {"STX", &MOS6502::STX, &MOS6502::ZPY, 4},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"TYA", &MOS6502::TYA, &MOS6502::IMP, 2},
      {"STA", &MOS6502::STA, &MOS6502::ABY, 5},
      {"TXS", &MOS6502::TXS, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 5},
      {"STA", &MOS6502::STA, &MOS6502::ABX, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"LDY", &MOS6502::LDY, &MOS6502::IMM, 2},
      {"LDA", &MOS6502::LDA, &MOS6502::IZX, 6},
      {"LDX", &MOS6502::LDX, &MOS6502::IMM, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"LDY", &MOS6502::LDY, &MOS6502::ZP0, 3},
      {"LDA", &MOS6502::LDA, &MOS6502::ZP0, 3},
      {"LDX", &MOS6502::LDX, &MOS6502::ZP0, 3},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 3},
      {"TAY", &MOS6502::TAY, &MOS6502::IMP, 2},
      {"LDA", &MOS6502::LDA, &MOS6502::IMM, 2},
      {"TAX", &MOS6502::TAX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"LDY", &MOS6502::LDY, &MOS6502::ABS, 4},
      {"LDA", &MOS6502::LDA, &MOS6502::ABS, 4},
      {"LDX", &MOS6502::LDX, &MOS6502::ABS, 4},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"BCS", &MOS6502::BCS, &MOS6502::REL, 2},
      {"LDA", &MOS6502::LDA, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"LDY", &MOS6502::LDY, &MOS6502::ZPX, 4},
      {"LDA", &MOS6502::LDA, &MOS6502::ZPX, 4},
      {"LDX", &MOS6502::LDX, &MOS6502::ZPY, 4},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"CLV", &MOS6502::CLV, &MOS6502::IMP, 2},
      {"LDA", &MOS6502::LDA, &MOS6502::ABY, 4},
      {"TSX", &MOS6502::TSX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"LDY", &MOS6502::LDY, &MOS6502::ABX, 4},
      {"LDA", &MOS6502::LDA, &MOS6502::ABX, 4},
      {"LDX", &MOS6502::LDX, &MOS6502::ABY, 4},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 4},
      {"CPY", &MOS6502::CPY, &MOS6502::IMM, 2},
      {"CMP", &MOS6502::CMP, &MOS6502::IZX, 6},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"CPY", &MOS6502::CPY, &MOS6502::ZP0, 3},
      {"CMP", &MOS6502::CMP, &MOS6502::ZP0, 3},
      {"DEC", &MOS6502::DEC, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"INY", &MOS6502::INY, &MOS6502::IMP, 2},
      {"CMP", &MOS6502::CMP, &MOS6502::IMM, 2},
      {"DEX", &MOS6502::DEX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"CPY", &MOS6502::CPY, &MOS6502::ABS, 4},
      {"CMP", &MOS6502::CMP, &MOS6502::ABS, 4},
      {"DEC", &MOS6502::DEC, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BNE", &MOS6502::BNE, &MOS6502::REL, 2},
      {"CMP", &MOS6502::CMP, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"CMP", &MOS6502::CMP, &MOS6502::ZPX, 4},
      {"DEC", &MOS6502::DEC, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"CLD", &MOS6502::CLD, &MOS6502::IMP, 2},
      {"CMP", &MOS6502::CMP, &MOS6502::ABY, 4},
      {"NOP", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"CMP", &MOS6502::CMP, &MOS6502::ABX, 4},
      {"DEC", &MOS6502::DEC, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"CPX", &MOS6502::CPX, &MOS6502::IMM, 2},
      {"SBC", &MOS6502::SBC, &MOS6502::IZX, 6},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"CPX", &MOS6502::CPX, &MOS6502::ZP0, 3},
      {"SBC", &MOS6502::SBC, &MOS6502::ZP0, 3},
      {"INC", &MOS6502::INC, &MOS6502::ZP0, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 5},
      {"INX", &MOS6502::INX, &MOS6502::IMP, 2},
      {"SBC", &MOS6502::SBC, &MOS6502::IMM, 2},
      {"NOP", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::SBC, &MOS6502::IMP, 2},
      {"CPX", &MOS6502::CPX, &MOS6502::ABS, 4},
      {"SBC", &MOS6502::SBC, &MOS6502::ABS, 4},
      {"INC", &MOS6502::INC, &MOS6502::ABS, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"BEQ", &MOS6502::BEQ, &MOS6502::REL, 2},
      {"SBC", &MOS6502::SBC, &MOS6502::IZY, 5},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 8},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"SBC", &MOS6502::SBC, &MOS6502::ZPX, 4},
      {"INC", &MOS6502::INC, &MOS6502::ZPX, 6},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 6},
      {"SED", &MOS6502::SED, &MOS6502::IMP, 2},
      {"SBC", &MOS6502::SBC, &MOS6502::ABY, 4},
      {"NOP", &MOS6502::NOP, &MOS6502::IMP, 2},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
      {"???", &MOS6502::NOP, &MOS6502::IMP, 4},
      {"SBC", &MOS6502::SBC, &MOS6502::ABX, 4},
      {"INC", &MOS6502::INC, &MOS6502::ABX, 7},
      {"???", &MOS6502::XXX, &MOS6502::IMP, 7},
  };
}

/*
 * Destruct MOS6502 instance.
 */
MOS6502::~MOS6502() {
  // Nothing to do.
}

/*
 * Forces the MOS6502 into a known state:
 * This is hard-wired inside the CPU. The registers are set to 0x00, the status
 * register is cleared except for unused bit which remains at 1.
 */
void MOS6502::HandleReset() {
  PC.OFFSET = Read(MOS6502::kResetVectorAddress);
  PC.PAGE = Read(MOS6502::kResetVectorAddress + 1);
  A.REGISTER = X.REGISTER = Y.REGISTER = P.REGISTER = 0x00;
  P.UNUSED = 1u;
  S.REGISTER = MOS6502::kInitialStackAddress;
  opcode_ = operand_ = 0x00;
  address_ = offset_ = 0x0000;
  cycles_delayed_ = 8u;
}

/*
 * Interrupt request:
 * The current instruction is allowed to finish and then the current program
 * counter is stored onthe stack.
 */
void MOS6502::HandleIRQ() {
  if (!P.IRQ_DISABLE) {
    Write(MOS6502::kStackBaseAddress + S.REGISTER--, PC.PAGE);
    Write(MOS6502::kStackBaseAddress + S.REGISTER--, PC.OFFSET);
    P.BRK_COMMAND = 0u;
    P.UNUSED = 1u;
    P.IRQ_DISABLE = 1u;
    Write(MOS6502::kStackBaseAddress + S.REGISTER--, P.REGISTER);
    PC.OFFSET = Read(MOS6502::kResetVectorAddress);
    PC.PAGE = Read(MOS6502::kResetVectorAddress + 1);
    address_ = MOS6502::kResetVectorAddress;
    cycles_delayed_ = 7u;
  }
}

/*
 * Non-Maskable Interrupt:
 * It behaves in exactly the same way as a regular IRQ, but reads the new
 * program counter address form another location.
 */
void MOS6502::HandleNMI() {
  Write(MOS6502::kStackBaseAddress + S.REGISTER--, PC.PAGE);
  Write(MOS6502::kStackBaseAddress + S.REGISTER--, PC.OFFSET);
  P.BRK_COMMAND = 0u;
  P.UNUSED = 1u;
  P.IRQ_DISABLE = 1u;
  Write(MOS6502::kStackBaseAddress + S.REGISTER--, P.REGISTER);
  PC.OFFSET = Read(MOS6502::kNMIVectorAddress);
  PC.PAGE = Read(MOS6502::kNMIVectorAddress + 1);
  address_ = MOS6502::kNMIVectorAddress;
  cycles_delayed_ = 8u;
}

/*
 * Perform one clock cycles worth of emulation:
 * Each instruction requires a variable number of clock cycles to execute. In my
 * emulation, I only care about the final result and so I perform the entire
 * computation in one hit.
 */
void MOS6502::HandleClock() {
  if (cycles_delayed_ == 0) {
    opcode_ = Read(PC.REGISTER++);
    P.UNUSED = 1u;
    cycles_delayed_ = lookup_[opcode_].cycles;
    cycles_delayed_ += (this->*lookup_[opcode_].addressing_mode)() & (this->*lookup_[opcode_].opcode)();
    P.UNUSED = 1u;
  }
  clock_count_++;
  cycles_delayed_--;
}

/*
 * Connect to the specified bus.
 */
void MOS6502::ConnectTo(bus::Bus* bus) {
  bus_ = bus;
}

/*
 * Read data from Bus at specified location.
 */
types::uint8_t MOS6502::Read(types::uint16_t address) {
  return bus_->Read(address, false);
}

/*
 * Write data into Bus at specified location.
 */
void MOS6502::Write(types::uint16_t address, types::uint8_t data) {
  bus_->Write(address, data);
}

/*
 * Source data used by the instruction into a convenient numeric variable.
 */
types::uint8_t MOS6502::Fetch() {
  if (!(lookup_[opcode_].addressing_mode == &MOS6502::IMP))
    operand_ = Read(address_);
  return operand_;
}

}  // namespace cpu
}  // namespace nes
