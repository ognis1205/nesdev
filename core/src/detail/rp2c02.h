/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
#define _NESDEV_CORE_DETAIL_RP2C02_H_
#include <cstdint>
#include "nesdev/core/ppu.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {

class RP2C02 final : public PPU {
 public:
  struct Registers {
    // $2000
    union {
      Byte value;
      Bitfield<0, 2, Byte> nametable;        // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
      Bitfield<2, 1, Byte> increment;        // VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
      Bitfield<3, 1, Byte> sprite_tile;      // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
      Bitfield<4, 1, Byte> background_tile;  // Background pattern table address (0: $0000; 1: $1000)
      Bitfield<5, 1, Byte> sprite_height;    // Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
      Bitfield<6, 1, Byte> ppu_master_slave; // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
      Bitfield<7, 1, Byte> nmi_enable;       // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
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
    } ppumask = {0x00};
    // $2002
    union {
      Byte value;
      Bitfield<0, 5, Byte> previous_lsb;    // Least significant bits previously written into a PPU register
      Bitfield<5, 1, Byte> sprite_overflow; // Sprite overflow
      Bitfield<6, 1, Byte> sprite_zero_hit; // Sprite 0 Hit
      Bitfield<7, 1, Byte> vblank_started;  // Vertical blank has started (0: not in vblank; 1: in vblank)
    } ppustatus = {0x00};
    // $2003 OAM read/write address
    union {
      Byte value;
    } oamaddr = {0x00};
    // $2004 OAM data read/write
    union {
      Byte value;
    } oamdata = {0x00};
    // $2005 fine scroll position (two writes: X scroll, Y scroll)
    union {
      Byte value;
    } ppuscroll = {0x00};
    // $2006 PPU read/write address (two writes: most significant byte, least significant byte)
    union {
      Byte value;
    } ppuaddr = {0x00};
    // $2007 PPU data read/write
    union {
      Byte value;
    } ppudata = {0x00};
    // $2014 OAM DMA high address
    union {
      Byte value;
    } oamdma = {0x00};
  };

 public:
  RP2C02(Registers* const registers, MMU* const mmu);

  ~RP2C02();

  void Tick() override;

  Byte Read(Address address) const override;

  void Write(Address address, Byte byte) override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Registers* const registers_;

  MMU* const mmu_;
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
