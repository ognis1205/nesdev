/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <time.h>
#include <gtest/gtest.h>
#include "nes/core/types.h"

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
    nes::core::Address value;
    nes::core::Bitfield<0, 8, nes::core::Address> offset;
    nes::core::Bitfield<8, 8, nes::core::Address> page;
  } pc_ = {0x0000};

  union {
    nes::core::Byte value;
    nes::core::Bitfield<0, 1, nes::core::Byte> carry;
    nes::core::Bitfield<1, 1, nes::core::Byte> zero;
    nes::core::Bitfield<2, 1, nes::core::Byte> irq_disable;
    nes::core::Bitfield<3, 1, nes::core::Byte> decimal_mode;
    nes::core::Bitfield<4, 1, nes::core::Byte> brk_command;
    nes::core::Bitfield<5, 1, nes::core::Byte> unused;
    nes::core::Bitfield<6, 1, nes::core::Byte> overflow;
    nes::core::Bitfield<7, 1, nes::core::Byte> negative;
  } p_ = {0x00};
};

TEST_F(TypesTest, Bitfield) {
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
