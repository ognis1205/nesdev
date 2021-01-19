/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <time.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/memory_banks/chip.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace detail {
namespace memory_banks {

class ChipTest : public testing::Test {
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

  detail::memory_banks::Chip<0x0000, 0x1FFF> memory_bank_{0x800};
};

TEST_F(ChipTest, HasValidAddress) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_TRUE(memory_bank_.HasValidAddress(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_FALSE(memory_bank_.HasValidAddress(i));
  }
}

TEST_F(ChipTest, Read) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(0x00, memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Read(i), InvalidAddress);
  }
}

TEST_F(ChipTest, Write) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    memory_bank_.Write(i, static_cast<Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(static_cast<Byte>(i >> 2), memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Write(i, static_cast<Byte>(i >> 2)), InvalidAddress);
  }
}

TEST_F(ChipTest, PtrTo) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    memory_bank_.Write(i, static_cast<Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(static_cast<Byte>(i >> 2), *(memory_bank_.PtrTo(i)));
    EXPECT_EQ(*(memory_bank_.PtrTo(i)), *(memory_bank_.PtrTo(i % 0x800u)));
  }
}

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
