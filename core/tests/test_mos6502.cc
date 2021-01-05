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
