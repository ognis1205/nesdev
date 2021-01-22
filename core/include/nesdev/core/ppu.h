/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PPU_H_
#define _NESDEV_CORE_PPU_H_
#include <cstddef>
#include <array>
#include <vector>
#include "nesdev/core/clock.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class PPU : public Clock {
 public:
  template <Address From, Address To>
  class Nametables final : public MemoryBank {
   public:
    static_assert(
      From <= To,
      "Start address must be greater than end address");

   public:
    Nametables(std::size_t size, ROM* const rom)
      : rom_{rom} {
      NESDEV_CORE_CASSERT((To - From + 1u) % size == 0, "Size does not match address range");
      data_[0].resize(size);
      data_[1].resize(size);
    }

    [[nodiscard]]
    bool HasValidAddress(Address address) const noexcept override {
      if constexpr (From == 0) return address <= To;
      else return address >= From && address <= To;
    }

    Byte Read(Address address) const override {
      if (HasValidAddress(address)) return *PtrTo(address);
      else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Read", address));
    }

    void Write(Address address, Byte byte) override {
      if (HasValidAddress(address)) *PtrTo(address) = byte;
      else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Write", address));
    }

    std::size_t Size() const override {
      return data_[0].size() + data_[1].size();
    }

    Byte* Data() override {
      NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to Nametables"));
    }

    const Byte* Data() const override {
      NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to Nametables"));
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Byte* PtrTo(Address address) {
      return const_cast<Byte*>(std::as_const(*this).PtrTo(address));
    }

    const Byte* PtrTo(Address address) const {
      address &= 0x0FFF;
      switch(rom_->header->Mirroring()) {
      case ROM::Header::Mirroring::HORIZONTAL:
	if (address >= 0x0000 && address <= 0x03FF)
	  return &data_[0].data()[address & 0x03FF];
	if (address >= 0x0400 && address <= 0x07FF)
	  return &data_[0].data()[address & 0x03FF];
	if (address >= 0x0800 && address <= 0x0BFF)
	  return &data_[1].data()[address & 0x03FF];
	if (address >= 0x0C00 && address <= 0x0FFF)
	  return &data_[1].data()[address & 0x03FF];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      case ROM::Header::Mirroring::VERTICAL:
	if (address >= 0x0000 && address <= 0x03FF)
	  return &data_[0].data()[address & 0x03FF];
	if (address >= 0x0400 && address <= 0x07FF)
	  return &data_[1].data()[address & 0x03FF];
	if (address >= 0x0800 && address <= 0x0BFF)
	  return &data_[0].data()[address & 0x03FF];
	if (address >= 0x0C00 && address <= 0x0FFF)
	  return &data_[1].data()[address & 0x03FF];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      default:
	NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible mirroring specified to ROM"));
      }
    }

  NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    ROM* const rom_;

    std::array<std::vector<Byte>, 2> data_;
  };

 public:
  virtual ~PPU() = default;

  virtual void Tick() override = 0;

  virtual Byte Read(Address address) = 0;

  virtual void Write(Address address, Byte byte) = 0;

 public:
  [[nodiscard]]
  std::size_t Cycle() const noexcept {
    return context_.cycle;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      cycle               = {0};
    }

    std::size_t cycle = {0};
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
