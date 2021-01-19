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

class ROMFactoryTest : public testing::Test {
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

  std::string super_mario_brothers_ = "core/tests/data/super_mario_brothers.nes";

  std::string zelda_ = "core/tests/data/zelda.nes";
};

TEST_F(ROMFactoryTest, SuperMarioBrothers) {
  ifs_.open(super_mario_brothers_, std::ifstream::binary);
  auto rom = ROMFactory::NROM(ifs_);

  EXPECT_TRUE (rom->header->HasValidMagic());
  EXPECT_TRUE (rom->header->HasPRGRam());
  EXPECT_FALSE(rom->header->ContainsPersistentMemory());
  EXPECT_FALSE(rom->header->ContainsTrainer());
  EXPECT_FALSE(rom->header->IgnoreMirroing());
  EXPECT_FALSE(rom->header->IsVSUnisystem());
  EXPECT_FALSE(rom->header->IsPlayChoice());
  EXPECT_FALSE(rom->header->HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,       rom->header->Format());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL, rom->header->Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,      rom->header->TVSystem());
  EXPECT_EQ(0,                                rom->header->Mapper());

  EXPECT_EQ(1 * 32768u, rom->header->SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u,  rom->header->SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u,  rom->header->SizeOfCHRRom());
  EXPECT_EQ(0u,         rom->header->SizeOfCHRRam());

  EXPECT_EQ(1 * 32768u, rom->chips->prg_rom->Size());
  EXPECT_EQ(1 * 8192u,  rom->chips->chr_rom->Size());
  EXPECT_EQ(1 * 8192u,  rom->chips->prg_ram->Size());
  EXPECT_EQ(0u,         rom->chips->chr_ram->Size());

  ifs_.close();
}

TEST_F(ROMFactoryTest, Zelda) {
  ifs_.open(zelda_, std::ifstream::binary);
  EXPECT_THROW(auto rom = ROMFactory::NROM(ifs_), InvalidROM);
  ifs_.close();
}

}  // namespace core
}  // namespace nesdev
