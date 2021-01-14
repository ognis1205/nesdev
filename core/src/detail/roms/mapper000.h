/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#define _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#include "nesdev/core/ines_header.h"
#include "nesdev/core/mapper.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class Mapper000 final : public nesdev::core::Mapper {
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
  Mapper000(const INESHeader& header);

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
    return address >= Mapper000::kNametableFrom && address <= Mapper000::kNametableTo;
  }
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
