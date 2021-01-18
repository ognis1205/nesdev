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
#include "mocks/rom_chips.h"

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

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
