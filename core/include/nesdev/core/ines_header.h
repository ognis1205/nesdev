/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_INES_HEADER_H_
#define _NESDEV_CORE_INES_HEADER_H_
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

 public:
  INESHeader(std::ifstream* const ifs);

  ~INESHeader();

  [[nodiscard]]
  constexpr bool HasValidMagic() const noexcept;

  [[nodiscard]]
  constexpr Byte SizeOfPRGRom() const noexcept;

  [[nodiscard]]
  constexpr Byte SizeOfCHRRom() const noexcept;

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
  constexpr Byte SizeOfPRGRam() const noexcept;

  [[nodiscard]]
  TVSystem TV() const;

  [[nodiscard]]
  bool HasPRGRam() const noexcept;

  [[nodiscard]]
  bool HasBusConflict() const noexcept;

 private:
  Byte magic_[4];        // FLAGS 0-3  : ("NES" followed by MS-DOS end-of-file)

  Byte prg_rom_size_;    // FLAGS 4    : Size of PRG ROM in 16 KB units.

  Byte chr_rom_size_;    // FLAGS 5    : Size of CHR ROM in 8 KB units.

  union {
    Byte value;
    Bitfield<0, 1, Byte> mirroring;
    Bitfield<1, 1, Byte> contains_persistent_memory;
    Bitfield<2, 1, Byte> contains_trainer;
    Bitfield<3, 1, Byte> ignore_mirroring;
    Bitfield<4, 4, Byte> mapper_nibble_lo;
  } flags6_;             // FLAGS 6    : Mapper, mirroring, battery, trainer.

  union {
    Byte value;
    Bitfield<0, 1, Byte> is_vs_unisystem;
    Bitfield<1, 1, Byte> is_play_choice;
    Bitfield<2, 2, Byte> is_nes20_format;
    Bitfield<4, 4, Byte> mapper_nibble_hi;
  } flags7_;             // FLAGS 7    : Mapper, VS/Playchoice, NES 2.0.

  Byte prg_ram_size_;    // FLAGS 8    : PRG-RAM size (rarely used extension).

  union {
    Byte value;
    Bitfield<0, 1, Byte> tv_system;
    Bitfield<1, 7, Byte> reserved;
  } flags9_;             // FLAGS 9    : TV system (rarely used extension).

  union {
    Byte value;
    Bitfield<0, 2, Byte> tv_system;
    Bitfield<4, 1, Byte> prg_ram_not_present;
    Bitfield<5, 1, Byte> has_bus_conflict;
  } flags10_;            // FLAGS 10   : TV system, PRG-RAM presence (unofficial, rarely used extension).

  Byte unused_[5];       // FLAGS 11-15: Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_INES_HEADER_H_
