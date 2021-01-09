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
#include "detail/mmu.h"
#include "mock_cpu.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace test {

class CPUTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  MockCPU cpu_;
};

TEST_F(CPUTest, Context) {
  //  EXPECT_EQ(0, cpu_.context_.cycle);
  EXPECT_EQ(0x00, cpu_.Fetched());
  //  EXPECT_EQ(0x00, cpu_.context_.opcode_byte);
  EXPECT_EQ(0x0000, cpu_.Addr());
  EXPECT_EQ(0x0000, cpu_.Ptr());
  EXPECT_FALSE(cpu_.CrossPage());
  EXPECT_FALSE(cpu_.IfRST());
  EXPECT_FALSE(cpu_.IfRST());
  EXPECT_FALSE(cpu_.IfNMI());
}

}  // namespace test
}  // namespace core
}  // namespace nesdev
