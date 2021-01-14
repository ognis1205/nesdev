/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_INES_HEADER_H_
#define _NESDEV_CORE_INES_HEADER_H_
#include <cstddef>
#include <fstream>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

enum class INESFormat : Byte {
  NES10,
  NES20
};

enum class Mirroring : Byte {
  HARDWARE,
  HORIZONTAL,
  VERTICAL,
  ONESCREEN_LO,
  ONESCREEN_HI
};

enum class TVSystem : Byte {
  NTSC,
  PAL,
  DUAL_COMPAT
};

/*
 * The iNES format header.
 * [SEE] https://wiki.nesdev.com/w/index.php/INES
 * [SEE] http://nesdev.com/neshdr20.txt
 * [SEE] https://nescartdb.com/
 */
class INESHeader {
 public:
  static const Address kTrainerEnd = {0x0200};

  static constexpr std::size_t k16KByte = 16 * 1024;

  static constexpr std::size_t k8KByte  =  8 * 1024;

 public:
  [[nodiscard]]
  bool HasValidMagic() const noexcept {
    return magic_[0] == 0x4E && magic_[1] == 0x45 && magic_[2] == 0x53 && magic_[3] == 0x1A;
  }

  [[nodiscard]]
  std::size_t SizeOfPRGRom() const noexcept {
    return prg_rom_chunks_ * INESHeader::k16KByte;
  }

  [[nodiscard]]
  std::size_t NumOfPRGBanks() const noexcept {
    return prg_rom_chunks_;
  }

  [[nodiscard]]
  std::size_t SizeOfCHRRom() const noexcept {
    return chr_rom_chunks_ * INESHeader::k8KByte;
  }

  [[nodiscard]]
  std::size_t NumOfCHRBanks() const noexcept {
    return chr_rom_chunks_;
  }

  [[nodiscard]]
  Mirroring Mirror() const noexcept {
    return flags6_.mirroring ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
  }

  [[nodiscard]]
  bool ContainsPersistentMemory() const noexcept {
    return flags6_.contains_persistent_memory;
  }

  [[nodiscard]]
  bool ContainsTrainer() const noexcept {
    return flags6_.contains_trainer;
  }

  [[nodiscard]]
  bool IgnoreMirroing() const noexcept {
    return flags6_.ignore_mirroring;
  }

  [[nodiscard]]
  bool IsVSUnisystem() const noexcept {
    return flags7_.is_vs_unisystem;
  }

  [[nodiscard]]
  bool IsPlayChoice() const noexcept {
    return flags7_.is_play_choice;
  }

  [[nodiscard]]
  INESFormat Format() const noexcept {
    switch (flags7_.ines_format) {
    case 0x10: return INESFormat::NES20;
    default  : return INESFormat::NES10;
    }
  }

  [[nodiscard]]
  Byte Mapper() const noexcept {
    return flags7_.mapper_nibble_hi << 4 | flags6_.mapper_nibble_lo;
  }

  [[nodiscard]]
  std::size_t SizeOfPRGRam() const noexcept {
    return prg_ram_chunks_ ? prg_ram_chunks_ * INESHeader::k8KByte : INESHeader::k8KByte;
  }

  [[nodiscard]]
  TVSystem TV() const {
    switch(flags10_.tv_system) {
    case 0:         return TVSystem::NTSC;
    case 2:         return TVSystem::PAL;
    case 1: case 3: return TVSystem::DUAL_COMPAT;
    default:        NESDEV_CORE_THROW(InvalidCartridge::Occur("Invalid TV system specified to iNES header"));
    }
  }

  [[nodiscard]]
  bool HasPRGRam() const noexcept {
    return !flags10_.prg_ram_not_present;
  }

  [[nodiscard]]
  bool HasBusConflict() const noexcept {
    return flags10_.has_bus_conflict;
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:  
  // FLAGS 0-3  : ("NES" followed by MS-DOS end-of-file)
  Byte magic_[4] = {0x00, 0x00, 0x00, 0x00};

  // FLAGS 4    : Size of PRG ROM in 16 KB units.
  Byte prg_rom_chunks_ = {0x00};

  // FLAGS 5    : Size of CHR ROM in 8 KB units.
  Byte chr_rom_chunks_ = {0x00};

  // FLAGS 6    : Mapper, mirroring, battery, trainer.
  union {
    Byte value;
    Bitfield<0, 1, Byte> mirroring;
    Bitfield<1, 1, Byte> contains_persistent_memory;
    Bitfield<2, 1, Byte> contains_trainer;
    Bitfield<3, 1, Byte> ignore_mirroring;
    Bitfield<4, 4, Byte> mapper_nibble_lo;
  } flags6_ = {0x00};

  // FLAGS 7    : Mapper, VS/Playchoice, NES 2.0.
  union {
    Byte value;
    Bitfield<0, 1, Byte> is_vs_unisystem;
    Bitfield<1, 1, Byte> is_play_choice;
    Bitfield<2, 2, Byte> ines_format;
    Bitfield<4, 4, Byte> mapper_nibble_hi;
  } flags7_ = {0x00};

  // FLAGS 8    : PRG-RAM size (rarely used extension).
  Byte prg_ram_chunks_ = {0x00};

  // FLAGS 9    : TV system (rarely used extension).
  union {
    Byte value;
    Bitfield<0, 1, Byte> tv_system;
    Bitfield<1, 7, Byte> reserved;
  } flags9_ = {0x00};

  // FLAGS 10   : TV system, PRG-RAM presence (unofficial, rarely used extension).
  union {
    Byte value;
    Bitfield<0, 2, Byte> tv_system;
    Bitfield<4, 1, Byte> prg_ram_not_present;
    Bitfield<5, 1, Byte> has_bus_conflict;
  } flags10_ = {0x00};

  // FLAGS 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
  Byte unused_[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_INES_HEADER_H_
