/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <filesystem>
#include <string>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "utils.h"

namespace nesdev {
namespace core {
namespace test {

class CartridgeFactoryTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  std::ifstream ifs_;

  std::string super_mario_brothers_ = "core/tests/data/super_mario_brothers.nes";

  std::string zelda_ = "core/tests/data/zelda.nes";
};

TEST_F(CartridgeFactoryTest, SuperMarioBrothers) {
  ifs_.open(super_mario_brothers_, std::ifstream::binary);
  auto cartridge = CartridgeFactory::Create(ifs_);
  EXPECT_TRUE(cartridge->header_.HasValidMagic());
  EXPECT_EQ(1 * 32768u, cartridge->header_.SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u, cartridge->header_.SizeOfCHRRom());
  EXPECT_EQ(Mirroring::VERTICAL, cartridge->header_.Mirror());
  EXPECT_FALSE(cartridge->header_.ContainsPersistentMemory());
  EXPECT_FALSE(cartridge->header_.ContainsTrainer());
  EXPECT_FALSE(cartridge->header_.IgnoreMirroing());
  EXPECT_FALSE(cartridge->header_.IsVSUnisystem());
  EXPECT_FALSE(cartridge->header_.IsPlayChoice());
  EXPECT_EQ(INESFormat::NES10, cartridge->header_.Format());
  EXPECT_EQ(0, cartridge->header_.Mapper());
  EXPECT_EQ(1 * 8192u, cartridge->header_.SizeOfPRGRam());
  EXPECT_EQ(TVSystem::NTSC, cartridge->header_.TV());
  EXPECT_TRUE(cartridge->header_.HasPRGRam());
  EXPECT_FALSE(cartridge->header_.HasBusConflict());
  ifs_.close();
}

TEST_F(CartridgeFactoryTest, Zelda) {
  ifs_.open(zelda_, std::ifstream::binary);
  auto cartridge = CartridgeFactory::Create(ifs_);
  EXPECT_TRUE(cartridge->header_.HasValidMagic());
  EXPECT_EQ(4 * 32768u, cartridge->header_.SizeOfPRGRom());
  EXPECT_EQ(0 * 8192u, cartridge->header_.SizeOfCHRRom());
  EXPECT_EQ(Mirroring::HORIZONTAL, cartridge->header_.Mirror());
  EXPECT_TRUE(cartridge->header_.ContainsPersistentMemory());
  EXPECT_FALSE(cartridge->header_.ContainsTrainer());
  EXPECT_FALSE(cartridge->header_.IgnoreMirroing());
  EXPECT_FALSE(cartridge->header_.IsVSUnisystem());
  EXPECT_FALSE(cartridge->header_.IsPlayChoice());
  EXPECT_EQ(INESFormat::NES10, cartridge->header_.Format());
  EXPECT_EQ(1, cartridge->header_.Mapper());
  EXPECT_EQ(1 * 8192u, cartridge->header_.SizeOfPRGRam());
  EXPECT_EQ(TVSystem::NTSC, cartridge->header_.TV());
  EXPECT_TRUE(cartridge->header_.HasPRGRam());
  EXPECT_FALSE(cartridge->header_.HasBusConflict());
  ifs_.close();
}

}  // namespace test
}  // namespace core
}  // namespace nesdev
