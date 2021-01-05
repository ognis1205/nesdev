/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <time.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "utils.h"

class TypesTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  union {
    nesdev::core::Address value;
    nesdev::core::Bitfield<0, 8, nesdev::core::Address> offset;
    nesdev::core::Bitfield<8, 8, nesdev::core::Address> page;
  } pc_ = {0x0000};

  union {
    nesdev::core::Byte value;
    nesdev::core::Bitfield<0, 1, nesdev::core::Byte> carry;
    nesdev::core::Bitfield<1, 1, nesdev::core::Byte> zero;
    nesdev::core::Bitfield<2, 1, nesdev::core::Byte> irq_disable;
    nesdev::core::Bitfield<3, 1, nesdev::core::Byte> decimal_mode;
    nesdev::core::Bitfield<4, 1, nesdev::core::Byte> brk_command;
    nesdev::core::Bitfield<5, 1, nesdev::core::Byte> unused;
    nesdev::core::Bitfield<6, 1, nesdev::core::Byte> overflow;
    nesdev::core::Bitfield<7, 1, nesdev::core::Byte> negative;
  } p_ = {0x00};
};

TEST_F(TypesTest, Assignment) {
  EXPECT_EQ(0x0000, pc_.value);
  EXPECT_EQ(0x00, pc_.offset);
  EXPECT_EQ(0x00, pc_.page);

  pc_.value = 0x0101;
  EXPECT_EQ(0x0101, pc_.value);
  EXPECT_EQ(0x01, pc_.offset);
  EXPECT_EQ(0x01, pc_.page);

  pc_.page = 0x00;
  EXPECT_EQ(0x0001, pc_.value);
  EXPECT_EQ(0x01, pc_.offset);
  EXPECT_EQ(0x00, pc_.page);

  pc_.offset = 0x00;
  EXPECT_EQ(0x0000, pc_.value);
  EXPECT_EQ(0x00, pc_.offset);
  EXPECT_EQ(0x00, pc_.page);

  EXPECT_EQ(0b00000000, p_.value);
  EXPECT_EQ(0b0, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.carry = true;
  EXPECT_EQ(0b00000001, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.zero = true;
  EXPECT_EQ(0b00000011, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.irq_disable = true;
  EXPECT_EQ(0b00000111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.decimal_mode = true;
  EXPECT_EQ(0b00001111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.brk_command = true;
  EXPECT_EQ(0b00011111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.unused = true;
  EXPECT_EQ(0b00111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.overflow = true;
  EXPECT_EQ(0b01111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b1, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.negative = true;
  EXPECT_EQ(0b11111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b1, p_.overflow);
  EXPECT_EQ(0b1, p_.negative);

  p_.value = false;
  EXPECT_EQ(0b00000000, p_.value);
  EXPECT_EQ(0b0, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);
}

TEST_F(TypesTest, Mask) {
  EXPECT_EQ(0b00000000, p_.value);
  EXPECT_EQ(0b0, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.carry.mask;
  EXPECT_EQ(0b00000001, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.zero.mask;
  EXPECT_EQ(0b00000011, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.irq_disable.mask;
  EXPECT_EQ(0b00000111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.decimal_mode.mask;
  EXPECT_EQ(0b00001111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.brk_command.mask;
  EXPECT_EQ(0b00011111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.unused.mask;
  EXPECT_EQ(0b00111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.overflow.mask;
  EXPECT_EQ(0b01111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b1, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);

  p_.value |= p_.negative.mask;
  EXPECT_EQ(0b11111111, p_.value);
  EXPECT_EQ(0b1, p_.carry);
  EXPECT_EQ(0b1, p_.zero);
  EXPECT_EQ(0b1, p_.irq_disable);
  EXPECT_EQ(0b1, p_.decimal_mode);
  EXPECT_EQ(0b1, p_.brk_command);
  EXPECT_EQ(0b1, p_.unused);
  EXPECT_EQ(0b1, p_.overflow);
  EXPECT_EQ(0b1, p_.negative);

  p_.value = false;
  EXPECT_EQ(0b00000000, p_.value);
  EXPECT_EQ(0b0, p_.carry);
  EXPECT_EQ(0b0, p_.zero);
  EXPECT_EQ(0b0, p_.irq_disable);
  EXPECT_EQ(0b0, p_.decimal_mode);
  EXPECT_EQ(0b0, p_.brk_command);
  EXPECT_EQ(0b0, p_.unused);
  EXPECT_EQ(0b0, p_.overflow);
  EXPECT_EQ(0b0, p_.negative);
}

TEST_F(TypesTest, BitwiseAlithmetics) {
  pc_.value <<= 1;
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_FALSE(pc_.offset);

  pc_.value >>= 1;
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_FALSE(pc_.page);

  pc_.offset = 0b10100000;
  pc_.value <<= 1;
  EXPECT_TRUE(pc_.page);
  EXPECT_EQ(0b00000001, pc_.page);
  EXPECT_EQ(0b01000000, pc_.offset);
  EXPECT_EQ(0b0000000101000000, pc_.value);

  pc_.page = 0b00000101;
  pc_.offset = 0b00000101;
  pc_.offset |= 0b00000010;
  EXPECT_EQ(0b00000101, pc_.page);
  EXPECT_EQ(0b00000111, pc_.offset);
  EXPECT_EQ(0b0000010100000111, pc_.value);

  pc_.value |= 0b1100000011000000;
  EXPECT_EQ(0b11000101, pc_.page);
  EXPECT_EQ(0b11000111, pc_.offset);
  EXPECT_EQ(0b1100010111000111, pc_.value);

  pc_.page = 0b00000101;
  pc_.offset = 0b00000101;
  pc_.offset &= 0b00000100;
  EXPECT_EQ(0b00000101, pc_.page);
  EXPECT_EQ(0b00000100, pc_.offset);
  EXPECT_EQ(0b0000010100000100, pc_.value);

  pc_.value &= 0b0000010100000000;
  EXPECT_EQ(0b00000101, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000010100000000, pc_.value);

  pc_.page = 0b00000101;
  pc_.offset = 0b00000101;
  pc_.offset ^= 0b00000100;
  EXPECT_EQ(0b00000101, pc_.page);
  EXPECT_EQ(0b00000001, pc_.offset);
  EXPECT_EQ(0b0000010100000001, pc_.value);

  pc_.value ^= 0b0000010100000000;
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b00000001, pc_.offset);
  EXPECT_EQ(0b0000000000000001, pc_.value);
}

TEST_F(TypesTest, InDecrement) {
  pc_.value = 0x0000;
  EXPECT_EQ(0, pc_.value++);
  EXPECT_EQ(1, pc_.value);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b00000001, pc_.offset);
  EXPECT_EQ(1, pc_.value--);
  EXPECT_EQ(0, pc_.value);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);

  EXPECT_EQ(1, ++pc_.value);
  EXPECT_EQ(1, pc_.value);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b00000001, pc_.offset);
  EXPECT_EQ(0, --pc_.value);
  EXPECT_EQ(0, pc_.value);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);

  pc_.value = 0x0000;
  EXPECT_EQ(0, pc_.page++);
  EXPECT_EQ(1, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000100000000, pc_.value);
  EXPECT_EQ(1, pc_.page--);
  EXPECT_EQ(0, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000000000000, pc_.value);

  EXPECT_EQ(1, ++pc_.page);
  EXPECT_EQ(1, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000100000000, pc_.value);
  EXPECT_EQ(0, --pc_.page);
  EXPECT_EQ(0, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000000000000, pc_.value);

  pc_.value = 0x0000;
  EXPECT_EQ(0, pc_.offset++);
  EXPECT_EQ(1, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000001, pc_.value);
  EXPECT_EQ(1, pc_.offset--);
  EXPECT_EQ(0, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000000, pc_.value);

  EXPECT_EQ(1, ++pc_.offset);
  EXPECT_EQ(1, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000001, pc_.value);
  EXPECT_EQ(0, --pc_.offset);
  EXPECT_EQ(0, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000000, pc_.value);

  pc_.value = 0xFF00;
  EXPECT_EQ(0xFF, pc_.page++);
  EXPECT_EQ(0, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_EQ(0, pc_.page--);
  EXPECT_EQ(0xFF, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b1111111100000000, pc_.value);

  EXPECT_EQ(0, ++pc_.page);
  EXPECT_EQ(0, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_EQ(0xFF, --pc_.page);
  EXPECT_EQ(0xFF, pc_.page);
  EXPECT_EQ(0b00000000, pc_.offset);
  EXPECT_EQ(0b1111111100000000, pc_.value);

  pc_.value = 0x00FF;
  EXPECT_EQ(0xFF, pc_.offset++);
  EXPECT_EQ(0, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_EQ(0, pc_.offset--);
  EXPECT_EQ(0xFF, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000011111111, pc_.value);

  EXPECT_EQ(0, ++pc_.offset);
  EXPECT_EQ(0, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000000000000, pc_.value);
  EXPECT_EQ(0xFF, --pc_.offset);
  EXPECT_EQ(0xFF, pc_.offset);
  EXPECT_EQ(0b00000000, pc_.page);
  EXPECT_EQ(0b0000000011111111, pc_.value);
}
