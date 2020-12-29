/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <memory>
#include <gtest/gtest.h>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/types.h"
#include "detail/mmu.h"

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
//  mmu.Add();
}
