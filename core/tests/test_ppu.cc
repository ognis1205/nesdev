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

  union {
    Address value;
    PPU::Shifter<Address> shift;
    Bitfield<0, 8, Address> lo;
    Bitfield<8, 8, Address> hi;
  } shifter_;
};

TEST_F(PPUTest, Context) {
//  EXPECT_EQ(0, ppu_.Cycle());
//  EXPECT_EQ(0x00, ppu_.Latched());
//  EXPECT_EQ(0x0000, ppu_.Addr());
}

TEST_F(PPUTest, ShifterAssignment) {
  auto x = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(x);
  EXPECT_EQ(x, shifter_.lo);

  auto y = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(y);
  EXPECT_EQ(y, shifter_.lo);
  EXPECT_EQ(x, shifter_.hi);

  auto z = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(z);
  EXPECT_EQ(z, shifter_.lo);
  EXPECT_EQ(y, shifter_.hi);
}

TEST_F(PPUTest, ShifterShift) {
  auto x = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(x);
  EXPECT_EQ(x, shifter_.lo);

  auto y = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(y);
  EXPECT_EQ(y, shifter_.lo);
  EXPECT_EQ(x, shifter_.hi);

  auto s = Utility::RandomByte<0x00, 0x0F>();
  Address e = shifter_.value << s;
  shifter_.shift <<= s;
  EXPECT_EQ(e, shifter_.value);
}

}  // namespace core
}  // namespace nesdev
