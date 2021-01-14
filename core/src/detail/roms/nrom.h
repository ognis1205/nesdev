/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#define _NESDEV_CORE_DETAIL_ROMS_NROM_H_
#include <array>
#include <vector>
#include "nesdev/core/rom.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class NROM final : private nesdev::core::ROM {
 public:
  NROM(const INESHeader& header, std::vector<Byte> prg_rom, std::vector<Byte> chr_rom);

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  class Mapper000 final : public nesdev::core::ROM::Mapper {
   public:
    static const Address kChrRomFrom    = {0x0000};

    static const Address kChrRomTo      = {0x1FFF};

    static const Address kNametableFrom = {0x2000};

    static const Address kNametableTo   = {0x3EFF};

    static const Address kPrgRamFrom    = {0x6000};

    static const Address kPrgRamTo      = {0x7FFF};

    static const Address kPrgRomFrom    = {0x8000};

    static const Address kPrgRomTo      = {0xFFFF};

   public:
    Mapper000(const INESHeader& header, const ROM::Info& info);

    [[nodiscard]]
    bool HasValidAddress(Space space, Address address) const noexcept override;

    [[nodiscard]]
    Address MapR(Mapper::Space space, Address address) const override;

    [[nodiscard]]
    Address MapW(Mapper::Space space, Address address) const override;

    void Reset() noexcept override;

    [[nodiscard]]
    Mirroring Mirror() const noexcept override;

    [[nodiscard]]
    bool IRQ() const noexcept override;

    void ClearIRQ() noexcept override;

    void Scanline() noexcept override;

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    static bool InPRG(Address address) noexcept {
      return address >= Mapper000::kPrgRamFrom && address <= Mapper000::kPrgRomTo;
    }

    static bool InCHR(Address address) noexcept {
      return address >= Mapper000::kChrRomFrom && address <= Mapper000::kChrRomTo;
    }

    static bool InNametable(Address address) noexcept {
      return address >= NROM::Mapper000::kNametableFrom && address <= NROM::Mapper000::kNametableTo;
    }
  };

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  [[nodiscard]]
  Address Map(Mapper::Space space, Address address) const override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:  
  std::vector<Byte> chr_rom_;

  std::array<std::array<Byte, 0x0400>, 2> nametables_;

  std::vector<Byte> prg_rom_;

  std::vector<Byte> prg_ram_;
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_NROM_H_
