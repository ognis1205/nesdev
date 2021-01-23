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
      : rom_{rom},
	size_{size} {
      NESDEV_CORE_CASSERT(
	((To - From + 1u) % size_ == 0) && (0x1000 % size_ == 0),
	"Size does not match address range");
      data_[0].resize(size_);
      data_[1].resize(size_);
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
      return size_;
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
      address %= 0x1000;
      switch(rom_->header->Mirroring()) {
      case ROM::Header::Mirroring::HORIZONTAL:
	if (address >= 0x0000 && address <= 0x03FF) return &data_[0x00].data()[address % Size()];
	if (address >= 0x0400 && address <= 0x07FF) return &data_[0x00].data()[address % Size()];
	if (address >= 0x0800 && address <= 0x0BFF) return &data_[0x01].data()[address % Size()];
	if (address >= 0x0C00 && address <= 0x0FFF) return &data_[0x01].data()[address % Size()];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      case ROM::Header::Mirroring::VERTICAL:
	if (address >= 0x0000 && address <= 0x03FF) return &data_[0x00].data()[address % Size()];
	if (address >= 0x0400 && address <= 0x07FF) return &data_[0x01].data()[address % Size()];
	if (address >= 0x0800 && address <= 0x0BFF) return &data_[0x00].data()[address % Size()];
	if (address >= 0x0C00 && address <= 0x0FFF) return &data_[0x01].data()[address % Size()];
	NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Nametables", address));
      default:
	NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible mirroring specified to ROM"));
      }
    }

  NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    ROM* const rom_;

    std::size_t size_;

    std::array<std::vector<Byte>, 0x02> data_;
  };

  template <typename T = Address>
  class Shifter {
   public:
    template <typename U>
    Shifter& operator<<=(const U& rhs) {
      value_ <<= rhs;
      return *this;
    }

    template <typename U>
    Shifter& operator()(const U& operand) {
      value_ = value_ << sizeof(operand) | operand;
      return *this;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    T value_;
  };

 public:
  explicit PPU(const std::vector<Byte>& palette)
    : palette_{palette} {};

  virtual ~PPU() = default;

  virtual void Tick() override = 0;

  virtual Byte Read(Address address) = 0;

  virtual void Write(Address address, Byte byte) = 0;

 public:
  void Connect(ROM* const rom) {
    NESDEV_CORE_CASSERT(rom != nullptr, "Invalid ROM specified to Connect");
    rom_ = rom;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      cycle           = {0};
      scanline        = {0};
      odd_frame       = false;
    }

    int cycle = {0};

    int scanline = {0};

    bool odd_frame = false;
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
  [[nodiscard]]
  int& Cycle() noexcept {
    return context_.cycle;
  }

  void Cycle(int cycle) noexcept {
    context_.cycle = cycle;
  }

  [[nodiscard]]
  int& Scanline() noexcept {
    return context_.scanline;
  }

  void Scanline(int scanline) noexcept {
    context_.scanline = scanline;
  }

  [[nodiscard]]
  bool IsOddFrame() const noexcept {
    return context_.odd_frame;
  }

  void TransitFrame() noexcept {
    context_.odd_frame = !context_.odd_frame;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsPreRenderOrVisibleLine() const noexcept {
    return context_.scanline >= -1 && context_.scanline < 240;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsPostRenderLine() const noexcept {
    return context_.scanline == 240;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsVerticalBlankingLine() const noexcept {
    return context_.scanline >= 241 && context_.scanline < 261;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsNotIdleCycle() const noexcept {
    return (context_.cycle >= 2 && context_.cycle < 258) || (context_.cycle >= 321 && context_.cycle < 338);
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsEndOfVisibleCycle() const noexcept {
    return context_.cycle == 256;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;

  Palette palette_;

  ROM* rom_ = nullptr;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
