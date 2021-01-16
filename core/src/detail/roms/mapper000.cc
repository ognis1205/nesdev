/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <vector>
#include "nesdev/core/rom.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/types.h"
#include "detail/roms/mapper000.h"

namespace {

const nesdev::core::AddressSpace<0x0000, 0x1FFF> kCHRRom;
const nesdev::core::AddressSpace<0x6000, 0x7FFF> kPRGRam;
const nesdev::core::AddressSpace<0x8000, 0xFFFF> kPRGRom;

}

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

Mapper000::Mapper000(ROM::Header* const header, ROM::Chips* const chips)
  : nesdev::core::ROM::Mapper(header, chips) {}

bool Mapper000::HasValidAddress(ROM::Mapper::Space space, Address address) const {
  switch (space) {
  case ROM::Mapper::Space::CPU:
    return ::kPRGRam.Contain(address) || ::kPRGRom.Contain(address);
  case ROM::Mapper::Space::PPU:
    return ::kCHRRom.Contain(address);
  default:
    NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address space specified", address));
  }
}

Byte Mapper000::Read(ROM::Mapper::Space space, Address address) const {
  switch (space) {
  // If PRGROM is 16KB
  //   CPU Address Bus          PRG ROM
  //   0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
  //   0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
  // If PRGROM is 32KB
  //   CPU Address Bus          PRG ROM
  //   0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
  case ROM::Mapper::Space::CPU:
    if (::kPRGRam.Contain(address))
      return chips_->prg_ram.at(address % header_->SizeOfPRGRam());
    if (::kPRGRom.Contain(address))
      return chips_->prg_rom.at(address % header_->SizeOfPRGRom());
    [[fallthrough]];
  case ROM::Mapper::Space::PPU:
    if (::kCHRRom.Contain(address))
      return chips_->chr_rom.at(address % header_->SizeOfCHRRom());
    [[fallthrough]];
  default:
    NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Read", address));
  }
}

Byte Mapper000::Write(Mapper::Space space, Address address, Byte byte) const {
  switch (space) {
  // If PRGROM is 16KB
  //   CPU Address Bus          PRG ROM
  //   0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
  //   0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
  // If PRGROM is 32KB
  //   CPU Address Bus          PRG ROM
  //   0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
  case Mapper::Space::CPU:
    if (::kPRGRam.Contain(address))
      return chips_->prg_ram[address % header_->SizeOfPRGRam()] = byte;
    [[fallthrough]];
  case Mapper::Space::PPU:
    if (!header_->NumOfCHRBanks() && ::kCHRRom.Contain(address))
      return chips_->chr_rom[address % header_->SizeOfCHRRom()] = byte;
    [[fallthrough]];
  default:
    NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Write", address));
  }
}

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
