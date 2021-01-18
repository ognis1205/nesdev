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
#include "detail/roms/nrom.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace test {

class ROMFactoryTest : public testing::Test {
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

TEST_F(ROMFactoryTest, SuperMarioBrothers) {
  ifs_.open(super_mario_brothers_, std::ifstream::binary);
  auto rom = ROMFactory::NROM(ifs_);
  EXPECT_TRUE(rom->header_->HasValidMagic());
  EXPECT_EQ(1 * 32768u, rom->header_->SizeOfPRGRom());
  EXPECT_EQ(1 * 32768u, rom->chips_->prg_rom->Size());
  EXPECT_EQ(1 * 8192u, rom->header_->SizeOfCHRRom());
  EXPECT_EQ(1 * 8192u, rom->chips_->chr_rom->Size());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL, rom->header_->Mirroring());
  EXPECT_FALSE(rom->header_->ContainsPersistentMemory());
  EXPECT_FALSE(rom->header_->ContainsTrainer());
  EXPECT_FALSE(rom->header_->IgnoreMirroing());
  EXPECT_FALSE(rom->header_->IsVSUnisystem());
  EXPECT_FALSE(rom->header_->IsPlayChoice());
  EXPECT_EQ(ROM::Header::Format::NES10, rom->header_->Format());
  EXPECT_EQ(0, rom->header_->Mapper());
  EXPECT_EQ(1 * 8192u, rom->header_->SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u, rom->chips_->prg_ram->Size());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC, rom->header_->TVSystem());
  EXPECT_TRUE(rom->header_->HasPRGRam());
  EXPECT_FALSE(rom->header_->HasBusConflict());
  ifs_.close();
}

TEST_F(ROMFactoryTest, Zelda) {
  ifs_.open(zelda_, std::ifstream::binary);
  EXPECT_THROW(auto rom = ROMFactory::NROM(ifs_), InvalidROM);
  ifs_.close();
}

}  // namespace test
}  // namespace core
}  // namespace nesdev
