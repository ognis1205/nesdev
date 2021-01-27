/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include <sstream>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/rp2a03.h"
#include "constants.h"
#include "utils.h"
#include "mocks/mmu.h"

namespace nesdev {
namespace core {
namespace detail {

class RP2A03Test : public testing::Test {
 protected:
  void SetUp() override {
    Utility::Init();
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  mocks::MMU mmu_;

  detail::RP2A03::Registers registers_;

  detail::RP2A03 rp2a03_{&registers_, &mmu_};
};

TEST_F(RP2A03Test, ALUShiftL) {
  auto ret = rp2a03_.ShiftL(0b10000000, true);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.ShiftL(0b01000000, true);
  EXPECT_EQ(0b10000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  registers_.p.carry    = false;
  registers_.p.zero     = false;
  registers_.p.negative = false;

  ret = rp2a03_.ShiftL(0b10000000, false);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.ShiftL(0b01000000, false);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUShiftR) {
  auto ret = rp2a03_.ShiftR(0b00000001, true);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.ShiftR(0b00000010, true);
  EXPECT_EQ(0b10000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  registers_.p.carry    = false;
  registers_.p.zero     = false;
  registers_.p.negative = false;

  ret = rp2a03_.ShiftR(0b00000001, false);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.carry);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.ShiftR(0b00000010, false);
  EXPECT_EQ(0b00000001, ret);
  EXPECT_FALSE(registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUIncrement) {
  auto ret = rp2a03_.Increment(0b00000001);
  EXPECT_EQ(0b00000010, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.Increment(0b01111111);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = rp2a03_.Increment(0b11111111);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUDecrement) {
  auto ret = rp2a03_.Decrement(0b00000001);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.Decrement(0b00000010);
  EXPECT_EQ(0b00000001, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.Decrement(0b00000000);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUPassThrough) {
  auto ret = rp2a03_.PassThrough(0b10000000);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = rp2a03_.PassThrough(0b00000000);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUOr) {
  auto ret = rp2a03_.Or(0b10101010, 0b01010101);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = rp2a03_.Or(0b00000000, 0b00000000);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUAnd) {
  auto ret = rp2a03_.And(0b10101010, 0b01010101);
  EXPECT_EQ(0b00000000, ret);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  ret = rp2a03_.And(0b10000000, 0b10000001);
  EXPECT_EQ(0b10000000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUXor) {
  auto ret = rp2a03_.Xor(0b10101010, 0b01010101);
  EXPECT_EQ(0b11111111, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  ret = rp2a03_.Xor(0b10100000, 0b11000000);
  EXPECT_EQ(0b01100000, ret);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUAdd) {
  auto ret = rp2a03_.Add(0x50, 0x10);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0x50, 0x50);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0x50, 0x90);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0x50, 0xD0);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0xD0, 0x10);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0xD0, 0x50);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0xD0, 0x90);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = 0;
  ret = rp2a03_.Add(0xD0, 0xD0);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}

TEST_F(RP2A03Test, ALUSub) {
  registers_.p.carry = true;
  auto ret = rp2a03_.Sub(0x50, 0xF0);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0x50, 0xB0);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0x50, 0x70);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0x50, 0x30);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0xD0, 0xF0);
  EXPECT_EQ(0xE0, ret);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0xD0, 0xB0);
  EXPECT_EQ(0x20, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0xD0, 0x70);
  EXPECT_EQ(0x60, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  registers_.p.carry = true;
  ret = rp2a03_.Sub(0xD0, 0x30);
  EXPECT_EQ(0xA0, ret);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}

TEST_F(RP2A03Test, ALUCmp) {
  rp2a03_.Cmp(0x50, 0xF0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  rp2a03_.Cmp(0x50, 0xB0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  rp2a03_.Cmp(0x50, 0x70);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  rp2a03_.Cmp(0x50, 0x30);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  rp2a03_.Cmp(0xD0, 0xF0);
  EXPECT_EQ(0b0, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);

  rp2a03_.Cmp(0xD0, 0xB0);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  rp2a03_.Cmp(0xD0, 0x70);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);

  rp2a03_.Cmp(0xD0, 0x30);
  EXPECT_EQ(0b1, registers_.p.carry);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
}

TEST_F(RP2A03Test, ALUBit) {
  rp2a03_.Bit(0b00000001, 0b11000000);
  EXPECT_TRUE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  rp2a03_.Bit(0b11111111, 0b11111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_TRUE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  rp2a03_.Bit(0b10000001, 0b01111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_TRUE(registers_.p.overflow);

  rp2a03_.Bit(0b10000001, 0b00111111);
  EXPECT_FALSE(registers_.p.zero);
  EXPECT_FALSE(registers_.p.negative);
  EXPECT_FALSE(registers_.p.overflow);
}

TEST_F(RP2A03Test, Next) {
  for (Word opcode = 0x00; opcode <= 0xFF; opcode++) {
    auto op = kOpcodes.at(opcode);
    rp2a03_.pipeline_.Clear();
    EXPECT_CALL(mmu_, Read(testing::_))
      .Times(1)
      .WillOnce(testing::Return(opcode));
    if (!op.cycles) {
//      EXPECT_THROW(rp2a03_.Next(), InvalidOpcode);
      rp2a03_.Next();
    } else {
      rp2a03_.Next();
      std::stringstream ss(Opcodes::ToString(rp2a03_.Op()));
      std::vector<std::string> decoded;
      while(ss.good()) {
	std::string s;
	getline(ss, s, ',');
	s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
	decoded.push_back(s);
      }
      EXPECT_EQ(op.inst, decoded[0]);
      EXPECT_EQ(op.addr, decoded[1]);
      EXPECT_THAT(
	rp2a03_.pipeline_.steps_.size(),
	testing::AllOf(testing::Ge(op.cycles - 1), testing::Le(op.cycles + 1)));
    }
  }
}

}  // namespace detail
}  // namespace core
}  // namespace nesdev
