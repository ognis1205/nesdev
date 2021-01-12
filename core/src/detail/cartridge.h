/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_CARTRIDGE_H_
#define _NESDEV_CORE_DETAIL_CARTRIDGE_H_
#include <array>
#include <vector>
#include "nesdev/core/cartridge.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {

class Cartridge final : public nesdev::core::Cartridge {
 public:
  Cartridge(const INESHeader& header, std::vector<Byte> prg_rom, std::vector<Byte> chr_rom);

 private:
  static const Address kChrRomTo      = {0x1FFF};

  static const Address kNametableFrom = {0x2000};

  static const Address kNametableTo   = {0x3EFF};

  static const Address kPrgRamFrom    = {0x6000};

  static const Address kPrgRamTo      = {0x7FFF};

  static const Address kPrgRomFrom    = {0x8000};

 private:
  std::vector<Byte> prg_rom_;

  std::vector<Byte> chr_rom_;

  std::vector<Byte> prg_ram_;

  std::array<std::array<Byte, 0x0400>, 2> nametables_;
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_CARTRIDGE_H_
