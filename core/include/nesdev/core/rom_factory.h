/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_ROM_FACTORY_H_
#define _NESDEV_CORE_ROM_FACTORY_H_
#include <iosfwd>
#include <memory>
#include "nesdev/core/rom.h"

namespace nesdev {
namespace core {

class ROMFactory {
 public:
  [[nodiscard]]
  static std::unique_ptr<ROM> NROM(std::istream &bytestream);
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_ROM_FACTORY_H_
