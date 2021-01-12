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

class INESHeaderTest : public testing::Test {
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

  INESHeader header_;

  std::string super_mario_brothers_header_ = "core/tests/data/super_mario_brothers_header.nes";

  std::string zelda_header_ = "core/tests/data/zelda_header.nes";
};

TEST_F(INESHeaderTest, SuperMarioBrothers) {
  ifs_.open(super_mario_brothers_header_, std::ifstream::binary);
  header_.Load(&ifs_);
  EXPECT_TRUE(header_.HasValidMagic());
  EXPECT_EQ(1 * 32768u, header_.SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u, header_.SizeOfCHRRom());
  EXPECT_EQ(Mirroring::VERTICAL, header_.Mirror());
  EXPECT_FALSE(header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_EQ(INESFormat::NES10, header_.Format());
  EXPECT_EQ(0, header_.Mapper());
  EXPECT_EQ(0, header_.SizeOfPRGRam());
  EXPECT_EQ(TVSystem::NTSC, header_.TV());
  EXPECT_TRUE(header_.HasPRGRam());
  EXPECT_FALSE(header_.HasBusConflict());
  ifs_.close();
}

TEST_F(INESHeaderTest, Zelda) {
  ifs_.open(zelda_header_, std::ifstream::binary);
  header_.Load(&ifs_);
  EXPECT_TRUE(header_.HasValidMagic());
  EXPECT_EQ(4 * 32768u, header_.SizeOfPRGRom());
  EXPECT_EQ(0 * 8192u, header_.SizeOfCHRRom());
  EXPECT_EQ(Mirroring::HORIZONTAL, header_.Mirror());
  EXPECT_TRUE(header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_EQ(INESFormat::NES10, header_.Format());
  EXPECT_EQ(1, header_.Mapper());
  EXPECT_EQ(0, header_.SizeOfPRGRam());
  EXPECT_EQ(TVSystem::NTSC, header_.TV());
  EXPECT_TRUE(header_.HasPRGRam());
  EXPECT_FALSE(header_.HasBusConflict());
  ifs_.close();
}

}  // namespace test
}  // namespace core
}  // namespace nesdev
