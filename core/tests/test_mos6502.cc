/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/mos6502.h"
#include "utils.h"

class MockMMU : public nesdev::core::MMU {
 public:
  MOCK_METHOD1(MockSet, void(nesdev::core::MemoryBanks));
  MOCK_CONST_METHOD1(Read, nesdev::core::Byte(nesdev::core::Address));
  MOCK_METHOD2(Write, void(nesdev::core::Address, nesdev::core::Byte));
  virtual void Set(nesdev::core::MemoryBanks memory_banks) noexcept {
    return MockSet(std::move(memory_banks));
  }
};

class MOS6502Test : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  MockMMU mmu_;

  nesdev::core::detail::MOS6502::Registers registers_;

  nesdev::core::detail::MOS6502 mos6502_{&registers_, &mmu_};

  struct Op {
    std::string inst;
    std::string addr;
    uint8_t cycles = 0;
  };

  std::vector<Op> lookup = {
    {"BRK", "IMM", 7}, {"ORA", "IZX", 6}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"ORA", "ZP0", 3}, {"ASL", "ZP0", 5}, {"???", "IMP", 0}, {"PHP", "IMP", 3}, {"ORA", "IMM", 2},
    {"ASL", "IMP", 2}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"ORA", "ABS", 0}, {"ASL", "ABS", 6}, {"???", "IMP", 0}, {"BPL", "REL", 2}, {"ORA", "IZY", 5}, {"???", "IMP", 0}, {"???", "IMP", 0},
    {"???", "IMP", 0}, {"ORA", "ZPX", 4}, {"ASL", "ZPX", 6}, {"???", "IMP", 0}, {"CLC", "IMP", 2}, {"ORA", "ABY", 4}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"ORA", "ABX", 4},
    {"ASL", "ABX", 7}, {"???", "IMP", 0}, {"JSR", "ABS", 6}, {"AND", "IZX", 6}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"BIT", "ZP0", 3}, {"AND", "ZP0", 3}, {"ROL", "ZP0", 5}, {"???", "IMP", 0},
    {"PLP", "IMP", 4}, {"AND", "IMM", 2}, {"ROL", "IMP", 2}, {"???", "IMP", 0}, {"BIT", "ABS", 4}, {"AND", "ABS", 4}, {"ROL", "ABS", 6}, {"???", "IMP", 0}, {"BMI", "REL", 2}, {"AND", "IZY", 5},
    {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"AND", "ZPX", 4}, {"ROL", "ZPX", 6}, {"???", "IMP", 0}, {"SEC", "IMP", 2}, {"AND", "ABY", 4}, {"???", "IMP", 0}, {"???", "IMP", 0},
    {"???", "IMP", 0}, {"AND", "ABX", 4}, {"ROL", "ABX", 7}, {"???", "IMP", 0}, {"RTI", "IMP", 6}, {"EOR", "IZX", 6}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"EOR", "ZP0", 3},
    {"LSR", "ZP0", 5}, {"???", "IMP", 0}, {"PHA", "IMP", 3}, {"EOR", "IMM", 2}, {"LSR", "IMP", 2}, {"???", "IMP", 0}, {"JMP", "ABS", 3}, {"EOR", "ABS", 4}, {"LSR", "ABS", 6}, {"???", "IMP", 0},
    {"BVC", "REL", 2}, {"EOR", "IZY", 5}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"EOR", "ZPX", 4}, {"LSR", "ZPX", 6}, {"???", "IMP", 0}, {"CLI", "IMP", 2}, {"EOR", "ABY", 4},
    {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"EOR", "ABX", 4}, {"LSR", "ABX", 7}, {"???", "IMP", 0}, {"RTS", "IMP", 6}, {"ADC", "IZX", 6}, {"???", "IMP", 0}, {"???", "IMP", 0},
    {"???", "IMP", 0}, {"ADC", "ZP0", 3}, {"ROR", "ZP0", 5}, {"???", "IMP", 0}, {"PLA", "IMP", 4}, {"ADC", "IMM", 2}, {"ROR", "IMP", 2}, {"???", "IMP", 0}, {"JMP", "IND", 5}, {"ADC", "ABS", 4},
    {"ROR", "ABS", 6}, {"???", "IMP", 0}, {"BVS", "REL", 2}, {"ADC", "IZY", 5}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"ADC", "ZPX", 4}, {"ROR", "ZPX", 6}, {"???", "IMP", 0},
    {"SEI", "IMP", 2}, {"ADC", "ABY", 4}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"ADC", "ABX", 4}, {"ROR", "ABX", 7}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"STA", "IZX", 6},
    {"???", "IMP", 0}, {"???", "IMP", 0}, {"STY", "ZP0", 3}, {"STA", "ZP0", 3}, {"STX", "ZP0", 3}, {"???", "IMP", 0}, {"DEY", "IMP", 2}, {"???", "IMP", 0}, {"TXA", "IMP", 2}, {"???", "IMP", 0},
    {"STY", "ABS", 4}, {"STA", "ABS", 4}, {"STX", "ABS", 4}, {"???", "IMP", 0}, {"BCC", "REL", 2}, {"STA", "IZY", 6}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"STY", "ZPX", 4}, {"STA", "ZPX", 4},
    {"STX", "ZPY", 4}, {"???", "IMP", 0}, {"TYA", "IMP", 2}, {"STA", "ABY", 5}, {"TXS", "IMP", 2}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"STA", "ABX", 5}, {"???", "IMP", 0}, {"???", "IMP", 0},
    {"LDY", "IMM", 2}, {"LDA", "IZX", 6}, {"LDX", "IMM", 2}, {"???", "IMP", 0}, {"LDY", "ZP0", 3}, {"LDA", "ZP0", 3}, {"LDX", "ZP0", 3}, {"???", "IMP", 0}, {"TAY", "IMP", 2}, {"LDA", "IMM", 2},
    {"TAX", "IMP", 2}, {"???", "IMP", 0}, {"LDY", "ABS", 4}, {"LDA", "ABS", 4}, {"LDX", "ABS", 4}, {"???", "IMP", 0}, {"BCS", "REL", 2}, {"LDA", "IZY", 5}, {"???", "IMP", 0}, {"???", "IMP", 0},
    {"LDY", "ZPX", 4}, {"LDA", "ZPX", 4}, {"LDX", "ZPY", 4}, {"???", "IMP", 0}, {"CLV", "IMP", 2}, {"LDA", "ABY", 4}, {"TSX", "IMP", 2}, {"???", "IMP", 0}, {"LDY", "ABX", 4}, {"LDA", "ABX", 4},
    {"LDX", "ABY", 4}, {"???", "IMP", 0}, {"CPY", "IMM", 2}, {"CMP", "IZX", 6}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"CPY", "ZP0", 3}, {"CMP", "ZP0", 3}, {"DEC", "ZP0", 5}, {"???", "IMP", 0},
    {"INY", "IMP", 2}, {"CMP", "IMM", 2}, {"DEX", "IMP", 2}, {"???", "IMP", 0}, {"CPY", "ABS", 4}, {"CMP", "ABS", 4}, {"DEC", "ABS", 6}, {"???", "IMP", 0}, {"BNE", "REL", 2}, {"CMP", "IZY", 5},
    {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"CMP", "ZPX", 4}, {"DEC", "ZPX", 6}, {"???", "IMP", 0}, {"CLD", "IMP", 2}, {"CMP", "ABY", 4}, {"NOP", "IMP", 2}, {"???", "IMP", 0},
    {"???", "IMP", 0}, {"CMP", "ABX", 4}, {"DEC", "ABX", 7}, {"???", "IMP", 0}, {"CPX", "IMM", 2}, {"SBC", "IZX", 6}, {"???", "IMP", 2}, {"???", "IMP", 0}, {"CPX", "ZP0", 3}, {"SBC", "ZP0", 3},
    {"INC", "ZP0", 5}, {"???", "IMP", 0}, {"INX", "IMP", 2}, {"SBC", "IMM", 2}, {"NOP", "IMP", 2}, {"???", "IMP", 0}, {"CPX", "ABS", 4}, {"SBC", "ABS", 4}, {"INC", "ABS", 6}, {"???", "IMP", 0},
    {"BEQ", "REL", 2}, {"SBC", "IZY", 5}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"SBC", "ZPX", 4}, {"INC", "ZPX", 6}, {"???", "IMP", 0}, {"SED", "IMP", 2}, {"SBC", "ABY", 4},
    {"NOP", "IMP", 2}, {"???", "IMP", 0}, {"???", "IMP", 0}, {"SBC", "ABX", 4}, {"INC", "ABX", 7}, {"???", "IMP", 0},
  };
};

TEST_F(MOS6502Test, ALUShiftL) {
  auto ret = mos6502_.ShiftL(0b10000000, true);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.ShiftL(0b01000000, true);
  EXPECT_EQ(0b10000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  registers_.p.carry    = false;
  registers_.p.zero     = false;
  registers_.p.negative = false;

  ret = mos6502_.ShiftL(0b10000000, false);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.ShiftL(0b01000000, false);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUShiftR) {
  auto ret = mos6502_.ShiftR(0b00000001, true);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.ShiftR(0b00000010, true);
  EXPECT_EQ(0b10000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  registers_.p.carry    = false;
  registers_.p.zero     = false;
  registers_.p.negative = false;

  ret = mos6502_.ShiftR(0b00000001, false);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.ShiftR(0b00000010, false);
  EXPECT_EQ(0b00000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUIncrement) {
  auto ret = mos6502_.Increment(0b00000001);
  EXPECT_EQ(0b00000010, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.Increment(0b01111111);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = mos6502_.Increment(0b11111111);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUDecrement) {
  auto ret = mos6502_.Decrement(0b00000001);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.Decrement(0b00000010);
  EXPECT_EQ(0b00000001, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.Decrement(0b00000000);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUPassThrough) {
  auto ret = mos6502_.PassThrough(0b10000000);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = mos6502_.PassThrough(0b00000000);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUOr) {
  auto ret = mos6502_.Or(0b10101010, 0b01010101);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = mos6502_.Or(0b00000000, 0b00000000);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUAnd) {
  auto ret = mos6502_.And(0b10101010, 0b01010101);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = mos6502_.And(0b10000000, 0b10000001);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUXor) {
  auto ret = mos6502_.Xor(0b10101010, 0b01010101);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = mos6502_.Xor(0b10100000, 0b11000000);
  EXPECT_EQ(0b01100000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUAdd) {
  auto ret = mos6502_.Add(0x50, 0x10);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0x50, 0x50);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0x50, 0x90);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0x50, 0xD0);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0xD0, 0x10);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0xD0, 0x50);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0xD0, 0x90);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = mos6502_.Add(0xD0, 0xD0);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}

TEST_F(MOS6502Test, ALUSub) {
  registers_.p.carry = true;
  auto ret = mos6502_.Sub(0x50, 0xF0);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0x50, 0xB0);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0x50, 0x70);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0x50, 0x30);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0xD0, 0xF0);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0xD0, 0xB0);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0xD0, 0x70);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = mos6502_.Sub(0xD0, 0x30);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}

TEST_F(MOS6502Test, ALUCmp) {
  mos6502_.Cmp(0x50, 0xF0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  mos6502_.Cmp(0x50, 0xB0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  mos6502_.Cmp(0x50, 0x70);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  mos6502_.Cmp(0x50, 0x30);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  mos6502_.Cmp(0xD0, 0xF0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  mos6502_.Cmp(0xD0, 0xB0);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  mos6502_.Cmp(0xD0, 0x70);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  mos6502_.Cmp(0xD0, 0x30);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(MOS6502Test, ALUBit) {
  mos6502_.Bit(0b00000001, 0b11000000);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  mos6502_.Bit(0b11111111, 0b11111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  mos6502_.Bit(0b10000001, 0b01111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  mos6502_.Bit(0b10000001, 0b00111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}
