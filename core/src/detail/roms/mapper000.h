/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#define _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace roms {

class Mapper000 final : public ROM::Mapper {
 public:
  Mapper000(ROM::Header* const header, ROM::Chips* const chips)
    : ROM::Mapper(header, chips) {}

  [[nodiscard]]
  bool HasValidAddress(ROM::Mapper::Space space, Address address) const override {
    switch (space) {
    case ROM::Mapper::Space::CPU:
      return chips_->prg_ram->HasValidAddress(address) || chips_->prg_rom->HasValidAddress(address);
    case ROM::Mapper::Space::PPU:
      return chips_->chr_ram->HasValidAddress(address) || chips_->chr_rom->HasValidAddress(address);
    default:
      NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address space specified to nesdev::core::detail::roms::Mapper000::HasValidAddress", address));
    }
  }

  Byte Read(ROM::Mapper::Space space, Address address) const override {
    switch (space) {
    case ROM::Mapper::Space::CPU:
      if (chips_->prg_ram->HasValidAddress(address))
        return chips_->prg_ram->Read(address);
      if (chips_->prg_rom->HasValidAddress(address))
        return chips_->prg_rom->Read(address);
      [[fallthrough]];
    case ROM::Mapper::Space::PPU:
      if (chips_->chr_ram->HasValidAddress(address))
        return chips_->chr_ram->Read(address);
      if (chips_->chr_rom->HasValidAddress(address))
        return chips_->chr_rom->Read(address);
      [[fallthrough]];
    default:
      if (space == ROM::Mapper::Space::CPU)
        NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::roms::Mapper000::Read from CPU", address));
      else
        NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::roms::Mapper000::Read from PPU", address));
    }
  }

  void Write(ROM::Mapper::Space space, Address address, Byte byte) const override {
    switch (space) {
    case ROM::Mapper::Space::CPU:
      if (chips_->prg_rom->HasValidAddress(address)) {
        chips_->prg_rom->Write(address, byte);        
        return;                                       
      }                                               
      if (chips_->prg_ram->HasValidAddress(address)) {
        chips_->prg_ram->Write(address, byte);
        return;
      }
      [[fallthrough]];
    case ROM::Mapper::Space::PPU:
      if (chips_->chr_rom->HasValidAddress(address)) {
        chips_->chr_rom->Write(address, byte);        
        return;                                       
      }                                               
      if (chips_->chr_ram->HasValidAddress(address)) {
        chips_->chr_ram->Write(address, byte);
        return;
      }
      [[fallthrough]];
    default:
      if (space == ROM::Mapper::Space::CPU)
        NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::roms::Mapper000::Write from CPU", address));
      else
        NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::roms::Mapper000::Write from PPU", address));
    }
  }

  [[nodiscard]]
  bool IRQ() const override {
    return false;
  }

  void ClearIRQ() override {
    // Do nothing.
  }

  void OnVisibleCycleEnds() override {
    // Do nothing.
  }

  void Reset() override {
    // Do nothing.
  }
};

}  // namespace roms
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_ROMS_MAPPER000_H_
