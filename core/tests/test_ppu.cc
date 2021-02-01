/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstring>
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

  PPU::ObjectAttributeMap<64> oam_;

  PPU::ObjectAttributeMap<>::Entry entry_;
};

TEST_F(PPUTest, Context) {
//  EXPECT_EQ(0, ppu_.Cycle());
//  EXPECT_EQ(0x00, ppu_.Latched());
//  EXPECT_EQ(0x0000, ppu_.Addr());
}

TEST_F(PPUTest, ObjectAttributeMapHasValidAddress) {
  for (auto i = 0x0000u; i <= 0x00FFu; i++) {
    EXPECT_TRUE(oam_.HasValidAddress(i));
  }
  for (auto i = 0x0100u; i <= 0xFFFFu; i++) {
    EXPECT_FALSE(oam_.HasValidAddress(i));
  }
}

TEST_F(PPUTest, ObjectAttributeMapRead) {
  for (auto i = 0x0000u; i <= 0x00FFu; i++) {
    EXPECT_EQ(0x00, oam_.Read(i));
  }
  for (auto i = 0x0100u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(oam_.Read(i), InvalidAddress);
  }
}

TEST_F(PPUTest, ObjectAttributeMapWrite) {
  auto byte = Utility::RandomByte<0x00, 0xFF>();
  for (auto i = 0x0000u; i <= 0x00FFu; i++) {
    oam_.Write(i, byte);
  }
  for (auto i = 0x0000u; i <= 0x00FFu; i++) {
    EXPECT_EQ(byte, oam_.Read(i));
  }
  for (auto i = 0u; i < 64u; i++) {
    auto entry = oam_.data_[i];
    EXPECT_EQ(byte, entry.y);
    EXPECT_EQ(byte, entry.id);
    EXPECT_EQ(byte, entry.attr);
    EXPECT_EQ(byte, entry.x);
  }
  for (auto i = 0x0100u; i <= 0xFFFFu; i++) {
    EXPECT_THROW(oam_.Write(i, byte), InvalidAddress);
  }

  auto addr = Utility::RandomByte<0x00, 0xFF>();
  addr &= ~0x03;
  auto y = Utility::RandomByte<0x00, 0xFF>();
  auto id = Utility::RandomByte<0x00, 0xFF>();
  auto attr = Utility::RandomByte<0x00, 0xFF>();
  auto x = Utility::RandomByte<0x00, 0xFF>();
  oam_.Write(addr + 0, y);
  oam_.Write(addr + 1, id);
  oam_.Write(addr + 2, attr);
  oam_.Write(addr + 3, x);
  auto entry = oam_.data_[addr / 4];
  EXPECT_EQ(y, entry.y);
  EXPECT_EQ(id, entry.id);
  EXPECT_EQ(attr, entry.attr);
  EXPECT_EQ(x, entry.x);
}

TEST_F(PPUTest, ObjectAttributeMapData) {
  auto addr = Utility::RandomByte<0x00, 0xFF>();
  addr &= ~0x03;
  auto y = Utility::RandomByte<0x00, 0xFF>();
  auto id = Utility::RandomByte<0x00, 0xFF>();
  auto attr = Utility::RandomByte<0x00, 0xFF>();
  auto x = Utility::RandomByte<0x00, 0xFF>();
  oam_.Write(addr + 0, y);
  oam_.Write(addr + 1, id);
  oam_.Write(addr + 2, attr);
  oam_.Write(addr + 3, x);
  std::memcpy(&entry_, &oam_.Data()[addr], sizeof(PPU::ObjectAttributeMap<>::Entry));
  EXPECT_EQ(y, entry_.y);
  EXPECT_EQ(id, entry_.id);
  EXPECT_EQ(attr, entry_.attr);
  EXPECT_EQ(x, entry_.x);
}

TEST_F(PPUTest, ShifterAssignment) {
  auto x = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(x);
  EXPECT_EQ(x, shifter_.lo);
  Address p = shifter_.hi;

  auto y = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(y);
  EXPECT_EQ(y, shifter_.lo);
  EXPECT_EQ(p, shifter_.hi);

  auto z = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift <<= 8u;
  shifter_.shift(z);
  EXPECT_EQ(z, shifter_.lo);
  EXPECT_EQ(y, shifter_.hi);
}

TEST_F(PPUTest, ShifterShift) {
  auto x = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(x);
  EXPECT_EQ(x, shifter_.lo);
  Address p = shifter_.hi;

  auto y = Utility::RandomByte<0x00, 0xFF>();
  shifter_.shift(y);
  EXPECT_EQ(y, shifter_.lo);
  EXPECT_EQ(p, shifter_.hi);

  auto s = Utility::RandomByte<0x00, 0x0F>();
  Address e = shifter_.value << s;
  shifter_.shift <<= s;
  EXPECT_EQ(e, shifter_.value);
}

}  // namespace core
}  // namespace nesdev
