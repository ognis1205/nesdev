/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
#define _NESDEV_CORE_DETAIL_RP2C02_H_
#include <cstdint>
#include <memory>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {

#define REG(x) registers_->x.value
#define MSK(x) registers_->ppustatus.x.mask

class RP2C02 final : public PPU {
 public:
  enum class MemoryMap : Address {
    PPUCTRL   = 0x0000,
    PPUMASK   = 0x0001,
    PPUSTATUS = 0x0002,
    OAMADDR   = 0x0003,
    OAMDATA   = 0x0004,
    PPUSCROLL = 0x0005,
    PPUADDR   = 0x0006,
    PPUDATA   = 0x0007
  };

  enum class SpriteSize : Byte {
    PIXELS_8x8,
    PIXELS_8x16
  };

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
      Bitfield<7, 1, Byte> vblank_start;    // Vertical blank has started (0: not in vblank; 1: in vblank)
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

  struct Chips {
    Chips(std::unique_ptr<MemoryBank> oam)
      : oam{std::move(oam)} {}

    const std::unique_ptr<MemoryBank> oam;
  };

 public:
  RP2C02(std::unique_ptr<Chips> chips, Registers* const registers, MMU* const mmu);

  ~RP2C02();

  void Tick() override;

  Byte Read(Address address) override;

  void Write(Address address, Byte byte) override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  class Latch {
   public:
    Latch(Context* const context, Registers* const registers, MMU* const mmu, Chips* const chips)
      : context_{context},
        registers_{registers},
        mmu_{mmu},
        chips_{chips} {}

      [[nodiscard]]

    Address BaseNameTblAddr() const {
      switch (registers_->ppuctrl.nametable) {
      case 0:
        return 0x2000;
      case 1:
        return 0x2400;
      case 2:
        return 0x2800;
      case 3:
        return 0x2C00;
      default:
        NESDEV_CORE_THROW(InvalidRegister::Occur("Invalid value specified to PPUCTRL/NAMETABLE", REG(ppuctrl)));
      }
    }

    [[nodiscard]]
    Address VRAMInc() const {
      switch (registers_->ppuctrl.increment) {
      case 0:
        return 0x0001;
      case 1:
        return 0x0020;
      default:
        NESDEV_CORE_THROW(InvalidRegister::Occur("Invalid value specified to PPUCTRL/INCREMENT", REG(ppuctrl)));
      }
    }

    [[nodiscard]]
    Address SpritePtrAddr() const {
      switch (registers_->ppuctrl.sprite_tile) {
      case 0:
        return 0x0000;
      case 1:
        return 0x1000;
      default:
        NESDEV_CORE_THROW(InvalidRegister::Occur("Invalid value specified to PPUCTRL/SPRITE_TILE", REG(ppuctrl)));
      }
    }

    [[nodiscard]]
    Address BackgroundPtrAddr() const {
      switch (registers_->ppuctrl.background_tile) {
      case 0:
        return 0x0000;
      case 1:
        return 0x1000;
      default:
        NESDEV_CORE_THROW(InvalidRegister::Occur("Invalid value specified to PPUCTRL/BACKGROUND_TILE", REG(ppuctrl)));
      }
    }

    [[nodiscard]]
    RP2C02::SpriteSize SpriteSize() const {
      switch (registers_->ppuctrl.sprite_height) {
      case 0:
        return RP2C02::SpriteSize::PIXELS_8x8;
      case 1:
        return RP2C02::SpriteSize::PIXELS_8x16;
      default:
        NESDEV_CORE_THROW(InvalidRegister::Occur("Invalid value specified to PPUCTRL/SPRITE_HEIGHT", REG(ppuctrl)));
      }
    }

    [[nodiscard]]
    bool IsMaster() const noexcept {
      return registers_->ppuctrl.ppu_master_slave;
    }

    [[nodiscard]]
    bool IsNMIEnable() const noexcept {
      return registers_->ppuctrl.nmi_enable;
    }

    void ReadPPUCtrl() const noexcept {
      /* Do nothing. */
    }

    void ReadPPUMask() const noexcept {
      /* Do nothing. */
    }
    
    void ReadPPUStatus() const noexcept {
      context_->latched     = (context_->latched & 0x1F) | (REG(ppustatus) & 0xE0);
      REG(ppustatus)       &= ~MSK(vblank_start);
      context_->is_latched  = false;
    }

    void ReadOAMAddr() const noexcept {
      /* Do nothing. */
    }

    void ReadOAMData() const {
      context_->latched = chips_->oam->Read(context_->oam.address);
    }

    void ReadPPUScroll() const noexcept {
      /* Do nothing. */
    }

    void ReadPPUAddr() const noexcept {
      /* Do nothing. */
    }

    void ReadPPUData() {
      // Reads from the NameTable ram get delayed one cycle, 
      // so output buffer which contains the data from the 
      // previous read request.
      if (context_->vram.address <= 0x3EFF) {
        context_->latched = deffered;
        deffered          = mmu_->Read(context_->vram.address);
      // However, if the address was in the palette range, the
      // data is not delayed, so it returns immediately.
      } else {
        deffered          = mmu_->Read(context_->vram.address);
        context_->latched = deffered;
      }
      context_->vram.address += VRAMInc();
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Context* const context_;

    Registers* const registers_;

    MMU* const mmu_;

    Chips* const chips_;

    Byte deffered = {0x00};
  };

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  static MemoryMap Map(Address address) {
    switch (address % 0x08) {
    case 0x0000:
      return MemoryMap::PPUCTRL;
    case 0x0001:
      return MemoryMap::PPUMASK;
    case 0x0002:
      return MemoryMap::PPUSTATUS;
    case 0x0003:
      return MemoryMap::OAMADDR;
    case 0x0004:
      return MemoryMap::OAMDATA;
    case 0x0005:
      return MemoryMap::PPUSCROLL;
    case 0x0006:
      return MemoryMap::PPUADDR;
    case 0x0007:
      return MemoryMap::PPUDATA;
    default:
      NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Map", address));
    }
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  [[nodiscard]]
  Address BaseNameTblAddr() const {
    return latch_.BaseNameTblAddr();
  }

  [[nodiscard]]
  Address VRAMInc() const noexcept {
    return latch_.VRAMInc();
  }

  [[nodiscard]]
  Address SpritePtrAddr() const noexcept {
    return latch_.SpritePtrAddr();
  }

  [[nodiscard]]
  Address BackgroundPtrAddr() const noexcept {
    return latch_.BackgroundPtrAddr();
  }

  [[nodiscard]]
  RP2C02::SpriteSize SpriteSize() const {
    return latch_.SpriteSize();
  }

  [[nodiscard]]
  bool IsMaster() const noexcept {
    return latch_.IsMaster();
  }

  [[nodiscard]]
  bool IsNMIEnable() const noexcept {
    return latch_.IsNMIEnable();
  }

  void ReadPPUCtrl() const noexcept {
    latch_.ReadPPUCtrl();
  }

  void ReadPPUMask() const noexcept {
    latch_.ReadPPUMask();
  }
    
  void ReadPPUStatus() const noexcept {
    latch_.ReadPPUStatus();
  }

  void ReadOAMAddr() const noexcept {
    latch_.ReadOAMAddr();
  }

  void ReadOAMData() const {
    latch_.ReadOAMData();
  }

  void ReadPPUScroll() const noexcept {
    latch_.ReadPPUScroll();
  }

  void ReadPPUAddr() const noexcept {
    latch_.ReadPPUAddr();
  }

  void ReadPPUData() {
    latch_.ReadPPUData();
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  const std::unique_ptr<Chips> chips_;

  Registers* const registers_;

  MMU* const mmu_;

  Latch latch_;
};

#undef REG
#undef MSK

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
