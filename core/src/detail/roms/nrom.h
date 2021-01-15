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
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class NROM final : private nesdev::core::ROM {
 public:
  NROM(const INESHeader& header, std::vector<Byte> prg_rom, std::vector<Byte> chr_rom);

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  [[nodiscard]]
  Address Map(Mapper::Space space, Address address) const override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:  
  std::vector<Byte> chr_rom_;

  std::vector<Byte> prg_rom_;

  std::vector<Byte> prg_ram_;
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
