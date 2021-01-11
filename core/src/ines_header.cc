/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <fstream>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"
#include "macros.h"

namespace nesdev {
namespace core {

INESHeader::INESHeader(std::ifstream* const ifs) {
  if (ifs->is_open()) ifs->read(reinterpret_cast<char*>(this), sizeof(*this));
  if (!HasValidMagic()) NESDEV_CORE_THROW(InvalidHeader::Occur("Incompatible file format to iNES"));
}

  [[nodiscard]]
  constexpr bool INESHeader::HasValidMagic() const noexcept {
    return magic_[0] == 0x4E && magic_[1] == 0x45 && magic_[2] == 0x53 && magic_[3] == 0x1A;
  }

  [[nodiscard]]
  constexpr Byte INESHeader::SizeOfPRGRom() const noexcept {
    return prg_rom_size_;
  }

  [[nodiscard]]
  constexpr Byte INESHeader::SizeOfCHRRom() const noexcept {
    return chr_rom_size_;
  }

  [[nodiscard]]
  Mirroring INESHeader::Mirror() const noexcept {
    return flags6_.mirroring ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
  }

  [[nodiscard]]
  bool INESHeader::ContainsPersistentMemory() const noexcept {
    return flags6_.contains_persistent_memory;
  }

  [[nodiscard]]
  bool INESHeader::ContainsTrainer() const noexcept {
    return flags6_.contains_trainer;
  }

  [[nodiscard]]
  bool INESHeader::IgnoreMirroing() const noexcept {
    return flags6_.ignore_mirroring;
  }

  [[nodiscard]]
  bool INESHeader::IsVSUnisystem() const noexcept {
    return flags7_.is_vs_unisystem;
  }

  [[nodiscard]]
  bool INESHeader::IsPlayChoice() const noexcept {
    return flags7_.is_play_choice;
  }

  [[nodiscard]]
  bool INESHeader::IsNES20Format() const noexcept {
    return flags7_.is_nes20_format == 2u;
  }

  [[nodiscard]]
  Byte INESHeader::Mapper() const noexcept {
    return flags7_.mapper_nibble_hi << 4 | flags6_.mapper_nibble_lo;
  }

  [[nodiscard]]
  constexpr Byte INESHeader::SizeOfPRGRam() const noexcept {
    return prg_ram_size_;
  }

  [[nodiscard]]
  TVSystem INESHeader::TV() const {
    switch(flags10_.tv_system) {
    case 0:
      return TVSystem::NTSC;
    case 2:
      return TVSystem::PAL;
    case 1:
    case 3:
      return TVSystem::DUAL_COMPAT;
    default:
      NESDEV_CORE_THROW(InvalidHeader::Occur("Invalid TV system specified to iNES header"));
    }
  }

  [[nodiscard]]
  bool INESHeader::HasPRGRam() const noexcept {
    return !flags10_.prg_ram_not_present;
  }

  [[nodiscard]]
  bool INESHeader::HasBusConflict() const noexcept {
    return flags10_.has_bus_conflict;
  }

}  // namespace core
}  // namespace nesdev
