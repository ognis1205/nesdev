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

  std::string donkey_kong_ = "core/tests/data/donkey_kong.nes";

  std::string super_mario_brothers_ = "core/tests/data/super_mario_brothers.nes";

  std::string tetris_ = "core/tests/data/tetris.nes";

  std::string zelda_ = "core/tests/data/zelda.nes";
};

TEST_F(ROMTest, DonkeyKong) {
  std::unique_ptr<ROM::Header> header = std::make_unique<ROM::Header>();

  ifs_.open(donkey_kong_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header->HasValidMagic());
  EXPECT_TRUE (header->HasPRGRam());
  EXPECT_FALSE(header->ContainsPersistentMemory());
  EXPECT_FALSE(header->ContainsTrainer());
  EXPECT_FALSE(header->IgnoreMirroing());
  EXPECT_FALSE(header->IsVSUnisystem());
  EXPECT_FALSE(header->IsPlayChoice());
  EXPECT_FALSE(header->HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header->Format());
  EXPECT_EQ(ROM::Header::Mirroring::HORIZONTAL, header->Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,        header->TVSystem());
  EXPECT_EQ(0,                                  header->Mapper());

  EXPECT_EQ(2 * 8192u, header->SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u, header->SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u, header->SizeOfCHRRom());
  EXPECT_EQ(0 * 8192u, header->SizeOfCHRRam());
}

TEST_F(ROMTest, SuperMarioBrothers) {
  std::unique_ptr<ROM::Header> header = std::make_unique<ROM::Header>();

  ifs_.open(super_mario_brothers_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header->HasValidMagic());
  EXPECT_TRUE (header->HasPRGRam());
  EXPECT_FALSE(header->ContainsPersistentMemory());
  EXPECT_FALSE(header->ContainsTrainer());
  EXPECT_FALSE(header->IgnoreMirroing());
  EXPECT_FALSE(header->IsVSUnisystem());
  EXPECT_FALSE(header->IsPlayChoice());
  EXPECT_FALSE(header->HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,       header->Format());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL, header->Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,      header->TVSystem());
  EXPECT_EQ(0,                                header->Mapper());

  EXPECT_EQ(1 * 32768u, header->SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u,  header->SizeOfPRGRam());
  EXPECT_EQ(1 * 8192u,  header->SizeOfCHRRom());
  EXPECT_EQ(0 * 8192u,  header->SizeOfCHRRam());
}

TEST_F(ROMTest, Tetris) {
  std::unique_ptr<ROM::Header> header = std::make_unique<ROM::Header>();

  ifs_.open(tetris_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header->HasValidMagic());
  EXPECT_TRUE (header->HasPRGRam());
  EXPECT_FALSE(header->ContainsPersistentMemory());
  EXPECT_FALSE(header->ContainsTrainer());
  EXPECT_FALSE(header->IgnoreMirroing());
  EXPECT_FALSE(header->IsVSUnisystem());
  EXPECT_FALSE(header->IsPlayChoice());
  EXPECT_TRUE (header->HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header->Format());
  EXPECT_EQ(ROM::Header::Mirroring::VERTICAL,   header->Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::DUAL_COMPAT, header->TVSystem());
  EXPECT_EQ(65,                                 header->Mapper());

  EXPECT_EQ(4   * 32768u, header->SizeOfPRGRom());
  EXPECT_EQ(105 * 8192u,  header->SizeOfPRGRam());
  EXPECT_EQ(16  * 8192u,  header->SizeOfCHRRom());
  EXPECT_EQ(0   * 8192u,  header->SizeOfCHRRam());
}

TEST_F(ROMTest, Zelda) {
  std::unique_ptr<ROM::Header> header = std::make_unique<ROM::Header>();

  ifs_.open(zelda_, std::ifstream::binary);
  ifs_.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));
  ifs_.close();

  EXPECT_TRUE (header->HasValidMagic());
  EXPECT_TRUE (header->HasPRGRam());
  EXPECT_TRUE (header->ContainsPersistentMemory());
  EXPECT_FALSE(header->ContainsTrainer());
  EXPECT_FALSE(header->IgnoreMirroing());
  EXPECT_FALSE(header->IsVSUnisystem());
  EXPECT_FALSE(header->IsPlayChoice());
  EXPECT_FALSE(header->HasBusConflict());

  EXPECT_EQ(ROM::Header::Format::NES10,         header->Format());
  EXPECT_EQ(ROM::Header::Mirroring::HORIZONTAL, header->Mirroring());
  EXPECT_EQ(ROM::Header::TVSystem::NTSC,        header->TVSystem());
  EXPECT_EQ(1,                                  header->Mapper());

  EXPECT_EQ(4 * 32768u, header->SizeOfPRGRom());
  EXPECT_EQ(1 * 8192u,  header->SizeOfPRGRam());
  EXPECT_EQ(0 * 8192u,  header->SizeOfCHRRom());
  EXPECT_EQ(1 * 8192u,  header->SizeOfCHRRam());
}

}  // namespace core
}  // namespace nesdev
