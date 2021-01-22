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
      return data_[0x00].size() + data_[0x01].size();
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
	if (address >= 0x0000 && address <= 0x03FF) return &data_[0x00].data()[address & 0x03FF];
	if (address >= 0x0400 && address <= 0x07FF) return &data_[0x00].data()[address & 0x03FF];
	if (address >= 0x0800 && address <= 0x0BFF) return &data_[0x01].data()[address & 0x03FF];
	if (address >= 0x0C00 && address <= 0x0FFF) return &data_[0x01].data()[address & 0x03FF];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      case ROM::Header::Mirroring::VERTICAL:
	if (address >= 0x0000 && address <= 0x03FF) return &data_[0x00].data()[address & 0x03FF];
	if (address >= 0x0400 && address <= 0x07FF) return &data_[0x01].data()[address & 0x03FF];
	if (address >= 0x0800 && address <= 0x0BFF) return &data_[0x00].data()[address & 0x03FF];
	if (address >= 0x0C00 && address <= 0x0FFF) return &data_[0x01].data()[address & 0x03FF];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      default:
	NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible mirroring specified to ROM"));
      }
    }

  NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    ROM* const rom_;

    std::array<std::vector<Byte>, 0x02> data_;
  };

 public:
  explicit PPU(const std::vector<Byte>& palette)
    : palette_{palette} {};

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
  /*
   *
   * [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering#Line-by-line_timing
   */
  enum class ScanlineTiming : Byte {
    PRERENDER,
    VISIBLE_0,
    VISIBLE_1_256,
    VISIBLE_257_320,
    VISIBLE_321_336,
    VISIBLE_337_340,
    POSTRENDER
  };

  struct Context {
    void Clear() {
      cycle    = {0};
      scanline = {0};
    }

    int cycle = {0};

    int scanline = {0};

    union {
      Address value;
      Bitfield< 0, 5, Address> coarse_x;
      Bitfield< 5, 5, Address> coarse_y;
      Bitfield<10, 1, Address> nametable_x;
      Bitfield<11, 1, Address> nametable_y;
      Bitfield<12, 3, Address> fine_y;
      Bitfield<15, 1, Address> unused;
      Bitfield< 0, 8, Address> lo;
      Bitfield< 8, 8, Address> hi;
    } timing = {0x0000};

    void Cycled() noexcept {
      ++cycle;
      if (cycle >= 341) {
        cycle = 0;
        ++scanline;
        if (scanline >= 261)
 	  scanline = -1;
      }
    }
  };

  /*
   * Predefined pallete stored in VGA Pallete format.
   * [SEE] https://wiki.nesdev.com/w/index.php/.pal
   */
  class Palette {
   public:
    static constexpr double kIntensityFactor = 0.3;

   public:
    Palette(const std::vector<Byte>& pal) {
      NESDEV_CORE_CASSERT(pal.size() == 0x40 * 3, "Size does not match palette size");

      for (std::size_t colour = 0x00; colour < 0x40; ++colour) {
	Byte r = pal[(colour * 3) + 0];
	Byte g = pal[(colour * 3) + 1];
	Byte b = pal[(colour * 3) + 2];
	data_[0][colour] = (r << 16) | (g << 8) | b;
      }

      for (std::size_t intensity = 0x01; intensity < 0x08; ++intensity) {
	for (std::size_t colour = 0x00; colour < 0x40; ++colour) {
	  double dr = static_cast<Byte>(data_[0][colour] >> 16);
	  double dg = static_cast<Byte>(data_[0][colour] >>  8);
	  double db = static_cast<Byte>(data_[0][colour] >>  0);
	  // Intensify red
	  if (intensity & 0x01) {
	    dr *= 1 + kIntensityFactor;
	    dg *= 1 - kIntensityFactor;
	    db *= 1 - kIntensityFactor;
	  }
	  // Intensify green
	  if (intensity & 0x02) {
	    dr *= 1 - kIntensityFactor;
	    dg *= 1 + kIntensityFactor;
	    db *= 1 - kIntensityFactor;
	  }
	  // Intensify blue
	  if (intensity & 0x04) {
	    dr *= 1 - kIntensityFactor;
	    dg *= 1 - kIntensityFactor;
	    db *= 1 + kIntensityFactor;
	  }
	  auto r = static_cast<Byte>(dr > 0xFF ? 0xFF : dr);
	  auto g = static_cast<Byte>(dg > 0xFF ? 0xFF : dg);
	  auto b = static_cast<Byte>(db > 0xFF ? 0xFF : db);
	  data_[intensity][colour] = (r << 16) | (g << 8) | b;
	}
      }
    };

    ~Palette() = default;

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    std::array<std::array<RGBA, 0x40>, 0x08> data_;
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;

  Palette palette_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
