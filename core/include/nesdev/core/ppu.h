/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PPU_H_
#define _NESDEV_CORE_PPU_H_
#include <climits>
#include <cstddef>
#include <algorithm>
#include <array>
#include <functional>
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
  static const std::size_t kNumSprites = 8;

  static const int kFrameW = 256;

  static const int kFrameH = 240;

 public:
//  using Framebuffer = RGBA[kFrameH][kFrameW];
//  using Framebuffer = RGBA[kFrameW * kFrameH];
  using PixelWriter = std::function<void(std::int16_t, std::int16_t, RGBA)>;

  /*
   * The following registers are defined according to the folloing Loopy's archetecture.
   * [SEE] https://wiki.nesdev.com/w/index.php/PPU_scrolling
   */
  struct Registers {
    // $2000
    union {
      Byte value;
      Bitfield<0, 2, Byte> nametable;                  // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
      Bitfield<0, 1, Byte> nametable_x;
      Bitfield<1, 1, Byte> nametable_y;
      Bitfield<2, 1, Byte> increment;                  // VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
      Bitfield<3, 1, Byte> sprite_tile;                // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
      Bitfield<4, 1, Byte> background_tile;            // Background pattern table address (0: $0000; 1: $1000)
      Bitfield<5, 1, Byte> sprite_height;              // Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
      Bitfield<6, 1, Byte> ppu_master_slave;           // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
      Bitfield<7, 1, Byte> nmi_enable;                 // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
    } ppuctrl = {0x00};
    // $2001
    union {
      Byte value;
      Bitfield<0, 1, Byte> greyscale;                  // Greyscale (0: normal color, 1: produce a greyscale display)
      Bitfield<1, 1, Byte> background_leftmost_enable; // 1: Show background in leftmost 8 pixels of screen, 0: Hide
      Bitfield<2, 1, Byte> sprite_leftmost_enable;     // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
      Bitfield<3, 1, Byte> background_enable;          // 1: Show background
      Bitfield<4, 1, Byte> sprite_enable;              // 1: Show sprites
      Bitfield<5, 1, Byte> emphasize_red;              // Emphasize red (green on PAL/Dendy)
      Bitfield<6, 1, Byte> emphasize_green;            // Emphasize green (red on PAL/Dendy)
      Bitfield<7, 1, Byte> emphasize_blue;             // Emphasize blue
      Bitfield<5, 3, Byte> intensity;
    } ppumask = {0x00};
    // $2002
    union {
      Byte value;
      Bitfield<0, 5, Byte> previous_lsb;               // Least significant bits previously written into a PPU register
      Bitfield<5, 1, Byte> sprite_overflow;            // Sprite overflow
      Bitfield<6, 1, Byte> sprite_zero_hit;            // Sprite 0 Hit
      Bitfield<7, 1, Byte> vblank_start;               // Vertical blank has started (0: not in vblank; 1: in vblank)
    } ppustatus = {0x00};
    // $2003 OAM read/write address
    union {
      Byte value;
    } oamaddr = {0x00};
    // $2004 OAM data read/write
    union {
      Byte value;
    } oamdata = {0x00};
    // $2005 fine scroll position (two writes: X scroll, Y scroll), CREDIT: Loopy
    union {
      Byte value;
    } fine_x = {0x00};
    // $2006 PPU read/write address (two writes: most significant byte, least significant byte), CREDIT: Loopy
    union {
      Address value;
      Bitfield< 0, 12, Address> tile_id;
      Bitfield< 0,  5, Address> coarse_x;
      Bitfield< 5,  5, Address> coarse_y;
      Bitfield<10,  1, Address> nametable_x;
      Bitfield<11,  1, Address> nametable_y;
      Bitfield<12,  3, Address> fine_y;
      Bitfield<15,  1, Address> unused;
      Bitfield< 0,  8, Address> lo;
      Bitfield< 8,  8, Address> hi;
    } vramaddr = {0x0000};
    // $2006 PPU read/write address (two writes: most significant byte, least significant byte), CREDIT: Loopy
    union {
      Address value;
      Bitfield< 0, 12, Address> tile_id;
      Bitfield< 0,  5, Address> coarse_x;
      Bitfield< 5,  5, Address> coarse_y;
      Bitfield<10,  1, Address> nametable_x;
      Bitfield<11,  1, Address> nametable_y;
      Bitfield<12,  3, Address> fine_y;
      Bitfield<15,  1, Address> unused;
      Bitfield< 0,  8, Address> lo;
      Bitfield< 8,  8, Address> hi;
    } tramaddr = {0x0000};
    // $2007 PPU data read/write
    union {
      Byte value;
    } ppudata = {0x00};
    // $2014 OAM DMA high address
    union {
      Byte value;
    } oamdma = {0x00};
  };

  template <typename T = Address>
  class Shifter {
   public:
    template <typename U>
    Shifter& operator<<=(const U& rhs) {
      NESDEV_CORE_CASSERT(
	CHAR_BIT * sizeof(T) > rhs,
	"Right operand is greater than or equal to the number of bits in the left operand");
      value_ <<= rhs;
      return *this;
    }

    template <typename U>
    Shifter& operator()(const U& operand) {
      NESDEV_CORE_CASSERT(
	CHAR_BIT * sizeof(T) > CHAR_BIT * sizeof(U),
	"Right operand is greater than or equal to the number of bits in the left operand");
      value_ = (value_ << CHAR_BIT * sizeof(U)) | operand;
      return *this;
    }

    operator unsigned() const {
      return value_;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    T value_;
  };

  struct Shifters {
    // Background tile pattern low bits
    union {
      Address value;
      Shifter<Address> shift;
    } background_pttr_lo = {0x0000};
    // Background tile pattern high bits
    union {
      Address value;
      Shifter<Address> shift;
    } background_pttr_hi = {0x0000};
    // Background tile palette attributes low bits
    union {
      Address value;
      Shifter<Address> shift;
    } background_attr_lo = {0x0000};
    // Background tile palette attributes high bits
    union {
      Address value;
      Shifter<Address> shift;
    } background_attr_hi = {0x0000};
    // Sprite pattern low bits
    union {
      Address value;
      Shifter<Address> shift;
    } sprite_pttr_lo[kNumSprites] = {{0x0000}};
    // Sprite pattern high bits
    union {
      Address value;
      Shifter<Address> shift;
    } sprite_pttr_hi[kNumSprites] = {{0x0000}};
  };

  struct Chips {
    Chips(std::unique_ptr<MemoryBank> oam)
      : oam{std::move(oam)} {}

    const std::unique_ptr<MemoryBank> oam;
  };

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
// TODO: Check if this assertion is unneccessary.      
//      NESDEV_CORE_CASSERT(
//	((To - From + 1u) % size_ == 0) && (0x1000 % size_ == 0),
//	"Size does not match address range");
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

  template <std::size_t Entries=64>
  class ObjectAttributeMap final : public MemoryBank {
   public:
    struct Entry {
      Byte y;
      Byte id;
      Byte attr;
      Byte x;
    };

   public:
    [[nodiscard]]
    bool HasValidAddress(Address address) const noexcept override {
      return address >= 0 && address < sizeof(Entry) * Entries;
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
      return Entries;
    }

    Byte* Data() override {
      return PtrTo(0);
    }

    const Byte* Data() const override {
      return PtrTo(0);
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Byte* PtrTo(Address address) {
      return const_cast<Byte*>(std::as_const(*this).PtrTo(address));
    }

    const Byte* PtrTo(Address address) const {
      return &reinterpret_cast<const Byte*>(data_)[address];
    }

  NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Entry data_[Entries];
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
    NESDEV_CORE_CASSERT(rom, "Invalid ROM specified to Connect");
    rom_ = rom;
  }

  void Framebuffer(PixelWriter pixel_writer) {
    context_.pixel_writer = pixel_writer;
  }

  [[nodiscard]]
  std::int16_t Cycle() noexcept {
    return context_.cycle;
  }

  [[nodiscard]]
  std::int16_t Scanline() noexcept {
    return context_.scanline;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsPreRenderOrVisibleLine() const noexcept {
    return context_.scanline >= -1 && context_.scanline < 240;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsEndOfVisibleCycle() const noexcept {
    return context_.cycle == 256;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsPostRenderLine() const noexcept {
    return context_.scanline == 240;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsStartOfIdleCycle() const noexcept {
    return context_.cycle == 257;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsNotIdleCycle() const noexcept {
    return (context_.cycle >= 2 && context_.cycle < 258) || (context_.cycle >= 321 && context_.cycle < 338);
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsStartOfVBlank() const noexcept {
    return context_.scanline == 241 && context_.cycle == 1;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsEndOfVBlank() const noexcept {
    return context_.scanline == -1 && context_.cycle >= 280 && context_.cycle < 305;
  }

  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  [[nodiscard]]
  bool IsEndOfScanline(bool superflous) const noexcept {
    if (superflous)
      return context_.cycle == 338 || context_.cycle == 340;
    else
      return context_.cycle == 340;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      cycle     = {0};
      scanline  = {0};
      odd_frame = false;
//      std::fill(&framebuffer[0][0], &framebuffer[0][0] + sizeof(framebuffer), 0);
//      std::fill(framebuffer, framebuffer + sizeof(framebuffer), 0);
    }

    std::int16_t cycle = {0};

    std::int16_t scanline = {0};

    bool odd_frame = false;

//    Framebuffer framebuffer = {};
    PixelWriter pixel_writer;
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

    RGBA Colour(Byte intensity, Byte colour) {
      return data_.at(intensity).at(colour);
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    std::array<std::array<RGBA, 0x40>, 0x08> data_;
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  void NextCycle() noexcept {
    context_.cycle++;
  }

 void Cycle(int cycle) noexcept {
    context_.cycle = cycle;
  }

  void NextScanline() noexcept {
    context_.scanline++;
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

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;

  Palette palette_;

  ROM* rom_ = nullptr;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
