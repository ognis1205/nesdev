/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#define _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#include <memory>
#include "nesdev/core/rom.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class NROM final : public ROM {
 public:
  NROM(std::unique_ptr<ROM::Header> header,
       std::unique_ptr<ROM::Chips> chips,
       std::unique_ptr<ROM::Mapper> mapper)
    : ROM(std::move(header),
          std::move(chips),
          std::move(mapper)) {}
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
