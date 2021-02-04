/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_ROM_H_
#define _NESDEV_CORE_ROM_H_
#include <memory>
#include <vector>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

struct ROM {
  /*
   * The iNES format header.
   * [SEE] https://wiki.nesdev.com/w/index.php/INES
   * [SEE] http://nesdev.com/neshdr20.txt
   * [SEE] https://nescartdb.com/
   */
  class Header {
   public:
    enum class Format : Byte {
      NES10,
      NES20
    };

    enum class Mirroring : Byte {
      HORIZONTAL,
      VERTICAL,
      HARDWARE,
      ONESCREEN_LO,
      ONESCREEN_HI
    };

    enum class TVSystem : Byte {
      NTSC,
      PAL,
      DUAL_COMPAT
    };

   public:
    [[nodiscard]]
    bool HasValidMagic() const {
      return magic_[0] == 0x4E && magic_[1] == 0x45 && magic_[2] == 0x53 && magic_[3] == 0x1A;
    }

    [[nodiscard]]
    std::size_t SizeOfPRGRom() const {
      return NumOfPRGRoms() * k16KByte;
    }

    [[nodiscard]]
    std::size_t NumOfPRGRoms() const {
      switch (Format()) {
      case Format::NES10:
        return prg_rom_chunks_;
      case Format::NES20:
        return (flags8_.prg_rom_chunks_nibble_hi << 8) | prg_rom_chunks_;
      default:
        NESDEV_CORE_THROW(InvalidROM::Occur("Invalid iNES format specified to iNES header"));
      }
    }

    [[nodiscard]]
    std::size_t SizeOfCHRRom() const {
      return NumOfCHRRoms() * k8KByte;
    }

    [[nodiscard]]
    std::size_t NumOfCHRRoms() const {
      switch (Format()) {
      case Format::NES10:
        return chr_rom_chunks_;
      case Format::NES20:
        return (flags8_.chr_rom_chunks_nibble_hi << 8) | prg_rom_chunks_;
      default:
        NESDEV_CORE_THROW(InvalidROM::Occur("Invalid iNES format specified to iNES header"));
      }
    }

    [[nodiscard]]
    std::size_t SizeOfPRGRam() const {
      return NumOfPRGRams() * k8KByte;
    }

    [[nodiscard]]
    std::size_t NumOfPRGRams() const {
      return flags8_.prg_ram_chunks == 0 ? 1 : flags8_.prg_ram_chunks;
    }

    [[nodiscard]]
    std::size_t SizeOfCHRRam() const {
      return NumOfCHRRams() * k8KByte;
    }

    [[nodiscard]]
    std::size_t NumOfCHRRams() const {
      return NumOfCHRRoms() == 0 ? 1 : 0;
    }

    [[nodiscard]]
    Header::Mirroring Mirroring() const {
      return flags6_.mirroring ? Header::Mirroring::VERTICAL : Header::Mirroring::HORIZONTAL;
    }

    [[nodiscard]]
    bool ContainsPersistentMemory() const {
      return flags6_.contains_persistent_memory;
    }

    [[nodiscard]]
    bool ContainsTrainer() const {
      return flags6_.contains_trainer;
    }

    [[nodiscard]]
    bool IgnoreMirroing() const {
      return flags6_.ignore_mirroring;
    }

    [[nodiscard]]
    bool IsVSUnisystem() const {
      return flags7_.is_vs_unisystem;
    }

    [[nodiscard]]
    bool IsPlayChoice() const {
      return flags7_.is_play_choice;
    }

    [[nodiscard]]
    Header::Format Format() const {
      switch (flags7_.ines_format) {
      case 0x10: return Header::Format::NES20;
      default  : return Header::Format::NES10;
      }
    }

    [[nodiscard]]
    Byte Mapper() const {
      return flags7_.mapper_nibble_hi << 4 | flags6_.mapper_nibble_lo;
    }

    [[nodiscard]]
    Header::TVSystem TVSystem() const {
      switch(flags10_.tv_system) {
      case 0:
        return Header::TVSystem::NTSC;
      case 2:
        return Header::TVSystem::PAL;
      case 1: case 3:
        return Header::TVSystem::DUAL_COMPAT;
      default:
        NESDEV_CORE_THROW(InvalidROM::Occur("Invalid TV system specified to iNES header"));
      }
    }

    [[nodiscard]]
    bool HasPRGRam() const {
      return !flags10_.prg_ram_not_present;
    }

    [[nodiscard]]
    bool HasBusConflict() const {
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
    union {
      Byte value;
      Bitfield<0, 8, Byte> prg_ram_chunks;
      Bitfield<0, 3, Byte> prg_rom_chunks_nibble_hi;
      Bitfield<3, 3, Byte> chr_rom_chunks_nibble_hi;
    } flags8_ = {0x00};

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

  struct Chips {
    Chips(std::unique_ptr<MemoryBank> chr_rom,
          std::unique_ptr<MemoryBank> chr_ram,
          std::unique_ptr<MemoryBank> prg_rom,
          std::unique_ptr<MemoryBank> prg_ram)
      : chr_rom{std::move(chr_rom)},
        chr_ram{std::move(chr_ram)},
        prg_rom{std::move(prg_rom)},
        prg_ram{std::move(prg_ram)} {};

    const std::unique_ptr<MemoryBank> chr_rom;

    const std::unique_ptr<MemoryBank> chr_ram;

    const std::unique_ptr<MemoryBank> prg_rom;

    const std::unique_ptr<MemoryBank> prg_ram;
  };

  class Mapper {
   public:
    enum class Space : Byte {
      CPU,
      PPU
    };

   public:
    explicit Mapper(Header* const header, Chips* const chips) : header_{header}, chips_{chips} {};

    virtual ~Mapper() = default;

    [[nodiscard]]
    virtual bool HasValidAddress(Space space, Address address) const = 0;

    virtual Byte Read(Space space, Address address) const = 0;

    virtual void Write(Space space, Address address, Byte byte) const = 0;

    [[nodiscard]]
    virtual bool IRQ() const = 0;

    virtual void ClearIRQ() = 0;

    virtual void OnVisibleCycleEnds() = 0;

    virtual void Reset() = 0;

   NESDEV_CORE_PROTECTED_UNLESS_TESTED:
    const Header* const header_;

    Chips* const chips_;
  };

  explicit ROM(std::unique_ptr<Header> header,
               std::unique_ptr<Chips> chips,
               std::unique_ptr<Mapper> mapper)
    : header{std::move(header)},
      mapper{std::move(mapper)},
      chips{std::move(chips)} {};

  virtual ~ROM() = default;

  const std::unique_ptr<const Header> header;

  const std::unique_ptr<Mapper> mapper;

  const std::unique_ptr<Chips> chips;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_ROM_H_
