/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PALETTES_H_
#define _NESDEV_CORE_PALETTES_H_
#include <vector>
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

/*
 * The following palettes are defined according to these references:
 * [SEE] https://wiki.nesdev.com/w/index.php/PPU_palettes
 */
class Palettes {
 public:
  [[nodiscard]]
  static std::vector<Byte> RP2C02() noexcept;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Palettes() {}
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PALETTES_H_
