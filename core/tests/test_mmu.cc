/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/mmu.h"

class MockMemoryBank : public nesdev::core::MemoryBank {
 public:
  MOCK_METHOD1(HasValidAddress, bool(const nesdev::core::Address&));
  MOCK_METHOD1(Read, nesdev::core::Byte(const nesdev::core::Address&));
  MOCK_METHOD2(Write, void(const nesdev::core::Address&, const nesdev::core::Byte&));
};

class MMUTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  nesdev::core::detail::MMU mmu;
};

TEST_F(MMUTest, Clear) {
  MockMemoryBank memory_bank1_, memory_bank2_;
//  mmu.Add();
}
