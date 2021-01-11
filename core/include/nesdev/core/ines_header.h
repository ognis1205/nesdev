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
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

enum class Mirroring : Byte {
  HORIZONTAL,
  VERTICAL
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
  static const Address kTrainerSize = {0x0200};

  static const Word k16kb = {0x4000};

  static const Word k8kb  = {0x2000};

 public:
  void Load(std::ifstream* const ifs);

  [[nodiscard]]
  bool HasValidMagic() const noexcept;

  [[nodiscard]]
  std::size_t SizeOfPRGRom() const noexcept;

  [[nodiscard]]
  std::size_t SizeOfCHRRom() const noexcept;

  [[nodiscard]]
  Mirroring Mirror() const noexcept;

  [[nodiscard]]
  bool ContainsPersistentMemory() const noexcept;

  [[nodiscard]]
  bool ContainsTrainer() const noexcept;

  [[nodiscard]]
  bool IgnoreMirroing() const noexcept;

  [[nodiscard]]
  bool IsVSUnisystem() const noexcept;

  [[nodiscard]]
  bool IsPlayChoice() const noexcept;

  [[nodiscard]]
  bool IsNES20Format() const noexcept;

  [[nodiscard]]
  Byte Mapper() const noexcept;

  [[nodiscard]]
  std::size_t SizeOfPRGRam() const noexcept;

  [[nodiscard]]
  TVSystem TV() const;

  [[nodiscard]]
  bool HasPRGRam() const noexcept;

  [[nodiscard]]
  bool HasBusConflict() const noexcept;

 public:
  // FLAGS 0-3  : ("NES" followed by MS-DOS end-of-file)
  Byte magic_[4] = {0x00, 0x00, 0x00, 0x00};

  // FLAGS 4    : Size of PRG ROM in 16 KB units.
  Byte prg_rom_size_ = {0x00};

  // FLAGS 5    : Size of CHR ROM in 8 KB units.
  Byte chr_rom_size_ = {0x00};

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
    Bitfield<2, 2, Byte> is_nes20_format;
    Bitfield<4, 4, Byte> mapper_nibble_hi;
  } flags7_ = {0x00};

  // FLAGS 8    : PRG-RAM size (rarely used extension).
  Byte prg_ram_size_ = {0x00};

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
