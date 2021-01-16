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

NROM::NROM(std::unique_ptr<Header> header, std::unique_ptr<ROM::Chips> chips, std::unique_ptr<ROM::Mapper> mapper)
  : nesdev::core::ROM(std::move(header), std::move(chips), std::move(mapper)) {}

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
