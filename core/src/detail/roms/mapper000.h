/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#define _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class Mapper000 final : public nesdev::core::ROM::Mapper {
 public:
  static const AddressSpace<0x0000, 0x1FFF> kCHRRom;

  static const AddressSpace<0x6000, 0x7FFF> kPRGRam;

  static const AddressSpace<0x8000, 0xFFFF> kPRGRom;

 public:
  Mapper000(ROM::Header* const header, ROM::Chips* const chips);

  [[nodiscard]]
  bool HasValidAddress(Space space, Address address) const override;

  Byte Read(Mapper::Space space, Address address) const override;

  void Write(Mapper::Space space, Address address, Byte byte) const override;

  [[nodiscard]]
  bool IRQ() const noexcept override {
    return false;
  }

  void ClearIRQ() noexcept override {
    // Do nothing.
  }

  void Scanline() noexcept override {
    // Do nothing.
  }

  void Reset() noexcept override {
    // Do nothing.
  }
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
