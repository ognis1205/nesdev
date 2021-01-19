/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <time.h>
#include <functional>
#include <vector>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/memory_banks/forward.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace detail {
namespace memory_banks {

class ForwardTest : public testing::Test {
 protected:
  void SetUp() override {
    Utility::Init();
    start_time_ = time(nullptr);
    data_.resize(0x800);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  std::vector<Byte> data_;

  std::function<Byte(Address)> reader_ = [this](Address address) {
    return data_.at(address % 0x800);
  };

  std::function<void(Address, Byte)> writer_ = [this](Address address, Byte byte) {
    data_[address % 0x800] = byte;
  };

  detail::memory_banks::Forward<0x0000, 0x1FFF> memory_bank_{0x800, reader_, writer_};
};

TEST_F(ForwardTest, HasValidAddress) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_TRUE(memory_bank_.HasValidAddress(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_FALSE(memory_bank_.HasValidAddress(i));
  }
}

TEST_F(ForwardTest, Read) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(0x00, memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Read(i), InvalidAddress);
  }
}

TEST_F(ForwardTest, Write) {
  auto byte = Utility::RandomByte<0x00, 0xFF>();
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    memory_bank_.Write(i, byte);
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    EXPECT_EQ(byte, memory_bank_.Read(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(memory_bank_.Write(i, byte), InvalidAddress);
  }
}

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
