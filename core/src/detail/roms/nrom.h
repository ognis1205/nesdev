/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#define _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#include <array>
#include <vector>
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class NROM final : public nesdev::core::ROM {
 public:
  NROM(ROM::Header* const header, ROM::Chips* const chips, ROM::Mapper* const mapper);
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
