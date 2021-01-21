/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_CHIPS_H_
#define _NESDEV_CORE_MOCKS_CHIPS_H_
#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/memory_banks/chip.h"
#include "detail/memory_banks/void.h"

namespace nesdev {
namespace core {
namespace mocks {

class Chips : public nesdev::core::ROM::Chips {
 public:
  Chips(bool is_void_chr_rom)
    : nesdev::core::ROM::Chips(
      MakeCHRRxm( is_void_chr_rom),
      MakeCHRRxm(!is_void_chr_rom),
      std::make_unique<detail::memory_banks::Chip<0x8000, 0xFFFF>>(32768u),
      std::make_unique<detail::memory_banks::Chip<0x6000, 0x7FFF>>(8192u)) {}

 private:
  std::unique_ptr<nesdev::core::MemoryBank> MakeCHRRxm(bool is_void_chr_rom) {
    if (is_void_chr_rom)
      return std::make_unique<detail::memory_banks::Void>();
    else
      return std::make_unique<detail::memory_banks::Chip<0x0000, 0x1FFF>>(8192u);
  }
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_CHIPS_H_
