/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <memory>
#include <time.h>
#include <gtest/gtest.h>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/types.h"
#include "detail/memory_bank.h"

class MemoryBankTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  nesdev::core::detail::MemoryBank<0x0000, 0x1FFF, 0x800> memory_bank_;
};

TEST_F(MemoryBankTest, HasValidAddress) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_TRUE(memory_bank_.HasValidAddress(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_FALSE(memory_bank_.HasValidAddress(i));
  }
}

TEST_F(MemoryBankTest, Read) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(0x00, memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Read(i), nesdev::core::InvalidAddress);
  }
}

TEST_F(MemoryBankTest, Write) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    memory_bank_.Write(i, static_cast<nesdev::core::Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(static_cast<nesdev::core::Byte>(i >> 2), memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Write(i, static_cast<nesdev::core::Byte>(i >> 2)), nesdev::core::InvalidAddress);
  }
}

TEST_F(MemoryBankTest, PointerTo) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    memory_bank_.Write(i, static_cast<nesdev::core::Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(static_cast<nesdev::core::Byte>(i >> 2), *(memory_bank_.PointerTo(i)));
    EXPECT_EQ(*(memory_bank_.PointerTo(i)), *(memory_bank_.PointerTo(i % 0x800u)));
  }
}
