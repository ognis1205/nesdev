/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "nesdev/core/rom.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/types.h"
#include "detail/roms/nrom.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

NROM::NROM(ROM::Header* const header, ROM::Chips* const chips, ROM::Mapper* const mapper)
  : nesdev::core::ROM(header, chips, mapper) {}

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
