/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <vector>
#include "nesdev/core/cartridge.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"
#include "detail/cartridge.h"

namespace nesdev {
namespace core {
namespace detail {

Cartridge::Cartridge(const INESHeader &header, std::vector<Byte> prg_rom, std::vector<Byte> chr_rom)
  : nesdev::core::Cartridge(header),
    prg_rom_(std::move(prg_rom)),
    chr_rom_(std::move(chr_rom)),
    prg_ram_(header_.SizeOfPRGRam()),
    nametables_() {}

}  // namespace detail
}  // namespace core
}  // namespace nesdev
