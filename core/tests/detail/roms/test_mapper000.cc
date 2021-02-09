/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/roms/mapper000.h"
#include "utils.h"
#include "mocks/chips.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class Mapper000Test : public testing::Test {
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

  std::unique_ptr<ROM::Header> header_ = std::make_unique<ROM::Header>();

  std::unique_ptr<ROM::Chips> mock_void_chr_rom_chips_ = std::make_unique<mocks::Chips>(true);

  std::unique_ptr<ROM::Chips> mock_fill_chr_rom_chips_ = std::make_unique<mocks::Chips>(false);
};

TEST_F(Mapper000Test, HasValidAddress) {
  auto mapper0 = detail::roms::Mapper000(header_.get(), mock_void_chr_rom_chips_.get());
  EXPECT_FALSE(mapper0.HasValidAddress(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x0000, 0x5FFF>()));
  EXPECT_TRUE (mapper0.HasValidAddress(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x6000, 0xFFFF>()));
  EXPECT_FALSE(mapper0.HasValidAddress(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x2000, 0xFFFF>()));
  EXPECT_TRUE (mapper0.HasValidAddress(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x0000, 0x1FFF>()));

  auto mapper1 = detail::roms::Mapper000(header_.get(), mock_fill_chr_rom_chips_.get());
  EXPECT_FALSE(mapper1.HasValidAddress(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x0000, 0x5FFF>()));
  EXPECT_TRUE (mapper1.HasValidAddress(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x6000, 0xFFFF>()));
  EXPECT_FALSE(mapper1.HasValidAddress(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x2000, 0xFFFF>()));
  EXPECT_TRUE (mapper1.HasValidAddress(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x0000, 0x1FFF>()));
}

TEST_F(Mapper000Test, ReadWithValidAddress) {
  auto mapper0 = detail::roms::Mapper000(header_.get(), mock_void_chr_rom_chips_.get());
  EXPECT_EQ(0x00, mapper0.Read(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x6000, 0xFFFF>()));
  EXPECT_EQ(0x00, mapper0.Read(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x0000, 0x1FFF>()));

  auto mapper1 = detail::roms::Mapper000(header_.get(), mock_fill_chr_rom_chips_.get());
  EXPECT_EQ(0x00, mapper1.Read(ROM::Mapper::Space::CPU, Utility::RandomAddress<0x6000, 0xFFFF>()));
  EXPECT_EQ(0x00, mapper1.Read(ROM::Mapper::Space::PPU, Utility::RandomAddress<0x0000, 0x1FFF>()));
}

TEST_F(Mapper000Test, WriteWithValidAddress) {
  auto mapper0 = detail::roms::Mapper000(header_.get(), mock_void_chr_rom_chips_.get());
  auto address = Utility::RandomAddress<0x6000, 0x7FFF>();
  auto byte    = Utility::RandomAddress<0x00, 0xFF>();
  mapper0.Write(ROM::Mapper::Space::CPU, address, byte);
  EXPECT_EQ(byte, mapper0.Read(ROM::Mapper::Space::CPU, address));
  address = Utility::RandomAddress<0x0000, 0x1FFF>();
  byte    = Utility::RandomAddress<0x00, 0xFF>();
  mapper0.Write(ROM::Mapper::Space::PPU, address, byte);
  EXPECT_EQ(byte, mapper0.Read(ROM::Mapper::Space::PPU, address));

  auto mapper1 = detail::roms::Mapper000(header_.get(), mock_fill_chr_rom_chips_.get());
  address = Utility::RandomAddress<0x6000, 0x7FFF>();
  byte    = Utility::RandomAddress<0x00, 0xFF>();
  mapper1.Write(ROM::Mapper::Space::CPU, address, byte);
  EXPECT_EQ(byte, mapper1.Read(ROM::Mapper::Space::CPU, address));
}

//TEST_F(Mapper000Test, WriteWithInvalidAddress) {
//  auto mapper0 = detail::roms::Mapper000(header_.get(), mock_void_chr_rom_chips_.get());
//  auto address = Utility::RandomAddress<0x8000, 0xFFFF>();
//  auto byte    = Utility::RandomAddress<0x00, 0xFF>();
//  EXPECT_THROW(mapper0.Write(ROM::Mapper::Space::CPU, address, byte), InvalidAddress);
//  address = Utility::RandomAddress<0x2000, 0xFFFF>();
//  byte    = Utility::RandomAddress<0x00, 0xFF>();
//  EXPECT_THROW(mapper0.Write(ROM::Mapper::Space::PPU, address, byte), InvalidAddress);
//
//  auto mapper1 = detail::roms::Mapper000(header_.get(), mock_fill_chr_rom_chips_.get());
//  address = Utility::RandomAddress<0x8000, 0xFFFF>();
//  byte    = Utility::RandomAddress<0x00, 0xFF>();
//  EXPECT_THROW(mapper1.Write(ROM::Mapper::Space::CPU, address, byte), InvalidAddress);
//  address = Utility::RandomAddress<0x0000, 0x1FFF>();
//  byte    = Utility::RandomAddress<0x00, 0xFF>();
//  EXPECT_THROW(mapper1.Write(ROM::Mapper::Space::PPU, address, byte), InvalidAddress);
//  address = Utility::RandomAddress<0x2000, 0xFFFF>();
//  byte    = Utility::RandomAddress<0x00, 0xFF>();
//  EXPECT_THROW(mapper1.Write(ROM::Mapper::Space::PPU, address, byte), InvalidAddress);
//}

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
