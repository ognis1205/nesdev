/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <time.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/memory_banks/void.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace detail {
namespace memory_banks {

class VoidTest : public testing::Test {
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

  detail::memory_banks::Void memory_bank_;
};

TEST_F(VoidTest, HasValidAddress) {
  for (auto i = 0x0000u; i <= 0xFFFFu; i++) {
    EXPECT_FALSE(memory_bank_.HasValidAddress(i));
  }
}

TEST_F(VoidTest, Read) {
  for (auto i = 0x0000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Read(i), InvalidAddress);
  }
}

TEST_F(VoidTest, Write) {
  auto byte = Utility::RandomByte<0x00, 0xFF>();
  for (auto i = 0x0000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Write(i, byte), InvalidAddress);
  }
}

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
