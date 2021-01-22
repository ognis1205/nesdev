/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PALETTES_H_
#define _NESDEV_CORE_PALETTES_H_
#include <vector>
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

/*
 * The following palettes are defined according to these references:
 * [SEE] https://wiki.nesdev.com/w/index.php/PPU_palettes
 */
enum Palette : Byte {
  PL2C02 // 2C02 PPU Palette
};

[[nodiscard]]
std::vector<Byte> Decode(Palette palette) noexcept;

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PALLETS_H_
