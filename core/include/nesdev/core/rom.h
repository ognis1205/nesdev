/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_ROM_H_
#define _NESDEV_CORE_ROM_H_
#include "nesdev/core/ines_header.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mapper.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class ROM {
 public:
  class Info {
   public:
    Address CHRRomStart() const noexcept { return chr_rom_start_; }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Address chr_rom_start_   = {0x0000};
    Address chr_rom_end_     = {0x0000};
    Address nametable_start_ = {0x0000};
    Address nametable_end_   = {0x0000};
    Address prg_ram_start_   = {0x0000};
    Address prg_ram_end_     = {0x0000};
    Address prg_rom_start_   = {0x0000};
    Address prg_rom_end_     = {0x0000};
  };

 public:
  explicit ROM(const INESHeader& header) : header_{header} {};

  virtual ~ROM() = default;

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  [[nodiscard]]
  virtual Address Map(Mapper::Space space, Address address) const = 0;

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  INESHeader header_;

  Info info_;

  Mapper* mapper_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_ROM_H_
