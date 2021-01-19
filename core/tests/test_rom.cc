/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <fstream>
#include <string>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "utils.h"

namespace nesdev {
namespace core {

class ROMTest : public testing::Test {
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

  std::ifstream ifs_;

  ROM::Header header_;

  std::string donkey_kong_ = "core/tests/data/donkey_kong.nes";

  std::string super_mario_brothers_ = "core/tests/data/super_mario_brothers.nes";

  std::string tetris_ = "core/tests/data/tetris.nes";

  std::string zelda_ = "core/tests/data/zelda.nes";
};

TEST_F(ROMTest, DonkeyKong) {
  ifs_.open(donkey_kong_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(&header_), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header_.HasValidMagic());
  EXPECT_TRUE (header_.HasPRGRam());
  EXPECT_FALSE(header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_FALSE(header_.HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header_.Format());
  EXPECT_EQ(ROM::Header::Mirroring::HORIZONTAL, header_.Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,        header_.TVSystem());
  EXPECT_EQ(0,                                  header_.Mapper());

  EXPECT_EQ(2 * 8192u, header_.SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u, header_.SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u, header_.SizeOfCHRRom());
  EXPECT_EQ(0 * 8192u, header_.SizeOfCHRRam());
}

TEST_F(ROMTest, SuperMarioBrothers) {
  ifs_.open(super_mario_brothers_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(&header_), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header_.HasValidMagic());
  EXPECT_TRUE (header_.HasPRGRam());
  EXPECT_FALSE(header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_FALSE(header_.HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,       header_.Format());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL, header_.Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,      header_.TVSystem());
  EXPECT_EQ(0,                                header_.Mapper());

  EXPECT_EQ(1 * 32768u, header_.SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u,  header_.SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u,  header_.SizeOfCHRRom());
  EXPECT_EQ(0 * 8192u,  header_.SizeOfCHRRam());
}

TEST_F(ROMTest, Tetris) {
  ifs_.open(tetris_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(&header_), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header_.HasValidMagic());
  EXPECT_TRUE (header_.HasPRGRam());
  EXPECT_FALSE(header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_TRUE (header_.HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header_.Format());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL,   header_.Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::DUAL_COMPAT, header_.TVSystem());
  EXPECT_EQ(65,                                 header_.Mapper());

  EXPECT_EQ(4   * 32768u, header_.SizeOfPRGRom());
  EXPECT_EQ(105 * 8192u,  header_.SizeOfPRGRam());
  EXPECT_EQ(16  * 8192u,  header_.SizeOfCHRRom());
  EXPECT_EQ(0   * 8192u,  header_.SizeOfCHRRam());
}

TEST_F(ROMTest, Zelda) {
  ifs_.open(zelda_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(&header_), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header_.HasValidMagic());
  EXPECT_TRUE (header_.HasPRGRam());
  EXPECT_TRUE (header_.ContainsPersistentMemory());
  EXPECT_FALSE(header_.ContainsTrainer());
  EXPECT_FALSE(header_.IgnoreMirroing());
  EXPECT_FALSE(header_.IsVSUnisystem());
  EXPECT_FALSE(header_.IsPlayChoice());
  EXPECT_FALSE(header_.HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header_.Format());
  EXPECT_EQ(ROM::Header::Mirroring::HORIZONTAL, header_.Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,        header_.TVSystem());
  EXPECT_EQ(1,                                  header_.Mapper());

  EXPECT_EQ(4 * 32768u, header_.SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u,  header_.SizeOfPRGRam());
  EXPECT_EQ(0 * 8192u,  header_.SizeOfCHRRom());
  EXPECT_EQ(1 * 8192u,  header_.SizeOfCHRRam());
}

}  // namespace core
}  // namespace nesdev
