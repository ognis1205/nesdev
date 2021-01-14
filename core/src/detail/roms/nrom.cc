/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <vector>
#include "nesdev/core/rom.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"
#include "detail/roms/nrom.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

NROM::Mapper000::Mapper000(const INESHeader& header)
  : nesdev::core::ROM::Mapper(std::move(header)) {}

bool NROM::Mapper000::HasValidAddress(Mapper::Space space, Address address) const noexcept {
  switch (space) {
  case Mapper::Space::CPU: return NROM::Mapper000::InPRG(address);
  case Mapper::Space::PPU: return NROM::Mapper000::InCHR(address) || NROM::Mapper000::InNametable(address);
  }
}

/*
 * If PRGROM is 16KB
 *   CPU Address Bus          PRG ROM
 *   0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
 *   0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
 * If PRGROM is 32KB
 *   CPU Address Bus          PRG ROM
 *   0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
 */
Address NROM::Mapper000::MapR(Mapper::Space space, Address from) const {
  switch (space) {
  case Mapper::Space::CPU:
    if (from >= NROM::Mapper000::kPrgRomFrom && from <= NROM::Mapper000::kPrgRomTo) {
      return from & (header_.NumOfPRGBanks() > 1 ? NROM::Mapper000::kPrgRamTo : 0x3FFF);
    }
  case Mapper::Space::PPU:
    return 0u;
  }
}

NROM::NROM(const INESHeader& header, std::vector<Byte> prg_rom, std::vector<Byte> chr_rom)
  : nesdev::core::ROM(header),
    chr_rom_(std::move(chr_rom)),
    nametables_(),
    prg_rom_(std::move(prg_rom)),
    prg_ram_(header_.SizeOfPRGRam()),
    mapper_(header_) {
  mapper_ = 
}

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
