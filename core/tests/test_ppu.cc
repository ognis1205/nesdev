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
#include "utils.h"
#include "mocks/ppu.h"

namespace nesdev {
namespace core {

class PPUTest : public testing::Test {
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

  mocks::PPU ppu_;
};

TEST_F(PPUTest, Context) {
//  EXPECT_EQ(0, ppu_.Cycle());
//  EXPECT_EQ(0x00, ppu_.Latched());
//  EXPECT_EQ(0x0000, ppu_.Addr());
}

TEST_F(PPUTest, Addr) {
//  EXPECT_FALSE(ppu_.IsLatched());
//  ppu_.Addr(0b11111111);
//  EXPECT_EQ(0b0011111100000000, ppu_.Addr());
//  EXPECT_TRUE(ppu_.IsLatched());
//  ppu_.Addr(0b11111111);
//  EXPECT_EQ(0b0011111111111111, ppu_.Addr());
//  EXPECT_FALSE(ppu_.IsLatched());
}

}  // namespace core
}  // namespace nesdev
