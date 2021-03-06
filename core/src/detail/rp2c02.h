/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
#define _NESDEV_CORE_DETAIL_RP2C02_H_
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <memory>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {

#define REG(x)              registers_->x.value
#define BIT(x, y)           registers_->x.y
#define MSK(x)              registers_->ppustatus.x.mask
#define BACK(x)             shifters_->background_##x.value
#define SHIFT_BACK(x, s)    shifters_->background_##x.shift <<= s
#define PUSH_BACK(x, s)     shifters_->background_##x.shift(s)
#define SPRT(x, e)          shifters_->sprite_##x[e].value
#define SHIFT_SPRT(x, e, s) shifters_->sprite_##x[e].shift <<= s
#define PUSH_SPRT(x, e, s)  shifters_->sprite_##x[e].shift(s)
#define FINE_X              (0x8000 >> REG(fine_x))

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

 public:
  RP2C02(PPU::Chips* const chips,
         PPU::Registers* const registers,
         PPU::Shifters* const shifters,
         MMU* const mmu,
         const std::vector<Byte>& colours);

  ~RP2C02();

  void Tick() override;

  Byte Read(Address address) override;

  void Write(Address address, Byte byte) override;

  bool IsRendering() const override {
    return BIT(ppumask, background_enable) || BIT(ppumask, sprite_enable);
  }

  Byte CtrlRegister() const override {
    return registers_->ppuctrl.value;
  }

  Byte MaskRegister() const override {
    return registers_->ppumask.value;
  }

  Address VRAMAddr() const override {
    return registers_->vramaddr.value;
  }

  Address TRAMAddr() const override {
    return registers_->tramaddr.value;
  }

  Address BgPttrLo() const override {
    return shifters_->background_pttr_lo.value;
  }

  Address BgPttrHi() const override {
    return shifters_->background_pttr_hi.value;
  }

  Address BgAttrLo() const override {
    return shifters_->background_attr_lo.value;
  }

  Address BgAttrHi() const override {
    return shifters_->background_attr_hi.value;
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  class Latch {
   public:
    Latch(PPU::Registers* const registers, MMU* const mmu, PPU::Chips* const chips)
      : registers_{registers},
        mmu_{mmu},
        chips_{chips} {}

    [[nodiscard]]
    Byte Latched() const {
      return latch_;
    }

    Byte Latched(Byte byte) {
      return latch_ = byte;
//      return byte;
    }

    [[nodiscard]]
    Byte Deffered() const {
      return deffered_;
    }

    Byte Deffered(Byte byte) {
      return deffered_ = byte;
    }

    void ReadPPUCtrl() {
      /* Do nothing. */
    }

    void ReadPPUMask() {
      /* Do nothing. */
    }
    
    void ReadPPUStatus() {
      Latched((Latched() & 0x1F) | (REG(ppustatus) & 0xE0));
      REG(ppustatus) &= ~MSK(vblank_start);
      is_latched_     = false;
    }

    void ReadOAMAddr() {
      /* Do nothing. */
    }

    void ReadOAMData() {
      Latched(chips_->oam->Read(REG(oamaddr)));
    }

    void ReadPPUScroll() {
      /* Do nothing. */
    }

    void ReadPPUAddr() {
      /* Do nothing. */
    }

    void ReadPPUData() {
      if (REG(vramaddr) <= 0x3EFF) {
        // Reads from the NameTable ram get delayed one cycle, 
        // so output buffer which contains the data from the 
        // previous read request.
        Latched(Deffered());
	Deffered(mmu_->Read(REG(vramaddr) & 0x3FFF));
      } else {
        // However, if the address was in the palette range, the
        // data is not delayed, so it returns immediately.
	Deffered(mmu_->Read(REG(vramaddr) & 0x3FFF));
        Latched(Deffered());
      }
      REG(vramaddr) += BIT(ppuctrl, increment) ? 0x0020 : 0x0001;
    }

    void WritePPUCtrl(Byte byte) {
      REG(ppuctrl)               = byte;
      BIT(tramaddr, nametable_x) = unsigned(BIT(ppuctrl, nametable_x));
      BIT(tramaddr, nametable_y) = unsigned(BIT(ppuctrl, nametable_y));
    }

    void WritePPUMask(Byte byte) {
      REG(ppumask) = byte;
    }

    void WritePPUStatus([[maybe_unused]]Byte byte) {
    }

    void WriteOAMAddr(Byte byte) {
      REG(oamaddr) = byte;
    }

    void WriteOAMData(Byte byte) {
      chips_->oam->Write(REG(oamaddr), byte);
    }

    void WritePPUScroll(Byte byte) {
      if (!is_latched_) {
        // First write to scroll register contains X offset in pixel space
        // which we split into coarse and fine x values
        REG(fine_x)             = byte & 0x07;
        BIT(tramaddr, coarse_x) = byte >> 3;
        is_latched_             = true;
      } else {
        // First write to scroll register contains Y offset in pixel space
        // which we split into coarse and fine Y values
        BIT(tramaddr, fine_y)   = byte & 0x07;
        BIT(tramaddr, coarse_y) = byte >> 3;
        is_latched_             = false;
      }
    }

    void WritePPUAddr(Byte byte) {
      if (!is_latched_) {
        // PPU address bus can be accessed by CPU via the ADDR and DATA
        // registers. The fisrt write to this register latches the high byte
        // of the address, the second is the low byte. Note the writes
        // are stored in the tram register.
        BIT(tramaddr, hi) = byte & 0x3F;
        is_latched_       = true;
      } else {
        // When a whole address has been written, the internal vram address
        // buffer is updated. Writing to the PPU is unwise during rendering
        // as the PPU will maintam the vram address automatically whilst
        // rendering the scanline position.
        BIT(tramaddr, lo) = byte;
        REG(vramaddr)     = REG(tramaddr);
        is_latched_       = false;
      }
    }

    void WritePPUData(Byte byte) {
      mmu_->Write(REG(vramaddr) & 0x3FFF, byte);
      REG(vramaddr) += BIT(ppuctrl, increment) ? 0x0020 : 0x0001;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    PPU::Registers* const registers_;

    MMU* const mmu_;

    PPU::Chips* const chips_;

    bool is_latched_ = false;

    Byte latch_ = {0x00};

    Byte deffered_ = {0x00};
  };

  class Shift {
   public:
    Shift(PPU::Context* const context,
          PPU::Colours* const colours,
          PPU::Registers* const registers,
          PPU::Shifters* const shifters,
          MMU* const mmu,
          PPU::Chips* const chips)
      : context_{context},
        colours_{colours},
        registers_{registers},
        shifters_{shifters},
        mmu_{mmu},
        chips_{chips} {}

    void UpdateAt(std::int16_t cycle) {
      if (BIT(ppumask, background_enable)) {
        SHIFT_BACK(pttr_lo, 1u);
        SHIFT_BACK(pttr_hi, 1u);
        SHIFT_BACK(attr_lo, 1u);
        SHIFT_BACK(attr_hi, 1u);
      }
      if (BIT(ppumask, sprite_enable) && cycle >= 1 && cycle < 258) {
        for (std::size_t i = 0; i < context_->num_sprites; i++) {
          if (context_->sprite[i].x > 0) {
            context_->sprite[i].x--;
          } else {
            SHIFT_SPRT(pttr_lo, i, 1u);
            SHIFT_SPRT(pttr_hi, i, 1u);
          }
        }
      }
    }

    void LoadBg() {
      PUSH_BACK(pttr_lo, static_cast<Byte>(context_->background.lsb));
      PUSH_BACK(pttr_hi, static_cast<Byte>(context_->background.msb));
      PUSH_BACK(attr_lo, static_cast<Byte>((context_->background.attr & 0x01) ? 0xFF : 0x00));
      PUSH_BACK(attr_hi, static_cast<Byte>((context_->background.attr & 0x02) ? 0xFF : 0x00));
    }

    void ReadBgId() {
      context_->background.id = mmu_->Read(0x2000 | BIT(vramaddr, tile_id));
    }

    void ReadBgAttr() {
      context_->background.attr = mmu_->Read(0x23C0
                                             | ( BIT(vramaddr, nametable_y)    << 11)
                                             | ( BIT(vramaddr, nametable_x)    << 10) 
                                             | ((BIT(vramaddr, coarse_y) >> 2) <<  3) 
                                             | ( BIT(vramaddr, coarse_x)       >>  2));
      // Since we know we can access a tile directly from the 12 bit address, we
      // can analyse the bottom bits of the coarse coordinates to provide us with
      // the correct offset into the 8-bit word, to yield the 2 bits we are
      // actually interested in which specifies the palette for the 2x2 group of
      // tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
      // Likewise if "coarse x % 4" < 2 we are in the left half else right half.
      // Ultimately we want the bottom two bits of our attribute word to be the
      // palette selected. So shift as required.
      if (BIT(vramaddr, coarse_y) & 0x02) context_->background.attr >>= 4;
      if (BIT(vramaddr, coarse_x) & 0x02) context_->background.attr >>= 2;
      context_->background.attr &= 0x03;
    }

    void ReadBgLSB() {
      context_->background.lsb = mmu_->Read((BIT(ppuctrl, background_tile) << 12)
                                   + (context_->background.id << 4)
                                   + BIT(vramaddr, fine_y));
    }

    void ReadBgMSB() {
      context_->background.msb = mmu_->Read((BIT(ppuctrl, background_tile) << 12)
                                            + (context_->background.id << 4)
                                            + BIT(vramaddr, fine_y)
                                            + 8);
    }

    void ClearSp() {
      for (std::size_t entry = 0; entry < PPU::kNumSprites; entry++) {
        SPRT(pttr_lo, entry) = 0x0000;
        SPRT(pttr_hi, entry) = 0x0000;
      }
    }

    void EvaluateSpAt(std::int16_t scanline) {
      // Clear sprites scanline with 0xFF, because 0xFF y coordinate is not visible.
      std::memset(context_->sprite,
                  0xFF,
                  PPU::kNumSprites * sizeof(PPU::ObjectAttributeMap<>::Entry));
      context_->num_sprites = 0;
      ClearSp();
      // Populate sprites to be rendered, that is, sprites which "collide" to the scanline.
      std::size_t entry = 0;
      may_sprite_zero_hit_ = false;
      while (entry < chips_->oam->Size() && context_->num_sprites < PPU::kNumSprites + 1) {
        Byte entry_addr = 4 * entry;
        // To evaluate "collide", compare y coordinate.
        std::int16_t diff = scanline - static_cast<std::int16_t>(chips_->oam->Read(entry_addr));
        if (diff >= 0 && diff < (Is8x8Mode() ? 8 : 16) && context_->num_sprites < PPU::kNumSprites) {
          std::memcpy(&context_->sprite[context_->num_sprites++],
                      &chips_->oam->Data()[entry_addr],
                      sizeof(PPU::ObjectAttributeMap<>::Entry));
          if (entry == 0) may_sprite_zero_hit_ = true;
        }
        entry++;
      }
      BIT(ppustatus, sprite_overflow) = (context_->num_sprites > PPU::kNumSprites);
    }

    void GatherSpAt(std::int16_t scanline) {
      for (std::size_t entry = 0; entry < context_->num_sprites; entry++) {
        Address addr;
        if (Is8x8Mode())
          addr = (BIT(ppuctrl, sprite_tile) << 12)
            | (context_->sprite[entry].id << 4)
            | (IsFlippedV(entry) ? (7 - (scanline - context_->sprite[entry].y)) : (scanline - context_->sprite[entry].y));
        else
          addr = ((context_->sprite[entry].id & 0x01) << 12)
            | ((IsTopHalf(scanline, entry) ? (context_->sprite[entry].id & 0xFE) : ((context_->sprite[entry].id & 0xFE) + 1)) << 4)
            | (IsFlippedV(entry) ? (7 - ((scanline - context_->sprite[entry].y) & 0x07)) : ((scanline - context_->sprite[entry].y) & 0x07));
        Byte pttr_lo = mmu_->Read(addr + 0);
        Byte pttr_hi = mmu_->Read(addr + 8);
        if (IsFlippedH(entry)) {
          // https://stackoverflow.com/a/2602885
          auto flip = [](Byte byte) {
            byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
            byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
            byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
            return byte;
          };
          pttr_lo = flip(pttr_lo);
          pttr_hi = flip(pttr_hi);
        }

        SPRT(pttr_lo, entry) = pttr_lo; 
        SPRT(pttr_hi, entry) = pttr_hi;
      }
    }

    void ComposeAt(std::int16_t cycle, std::int16_t scanline) {
      Byte bg_pix = 0x00;
      Byte bg_pal = 0x00;
      if (BIT(ppumask, background_enable) && (BIT(ppumask, background_leftmost_enable) || cycle >= 9)) {
        bg_pix = (static_cast<Byte>((BACK(pttr_hi) & FINE_X) > 0) << 1) | static_cast<Byte>((BACK(pttr_lo) & FINE_X) > 0);
        bg_pal = (static_cast<Byte>((BACK(attr_hi) & FINE_X) > 0) << 1) | static_cast<Byte>((BACK(attr_lo) & FINE_X) > 0);
      }
      Byte fg_pix = 0x00;
      Byte fg_pal = 0x00;
      [[maybe_unused]]Byte fg_pri = 0x00;
      if (BIT(ppumask, sprite_enable) && (BIT(ppumask, sprite_leftmost_enable) || (cycle >= 9))) {
        sprite_zero_rendered_ = false;
        for (std::size_t entry = 0; entry < context_->num_sprites; entry++) {
          if (context_->sprite[entry].x == 0) {
            fg_pix = (static_cast<Byte>((SPRT(pttr_hi, entry) & 0x80) > 0) << 1) | static_cast<Byte>((SPRT(pttr_lo, entry) & 0x80) > 0);
            fg_pal = (context_->sprite[entry].attr & 0x03) + 0x04;
            fg_pri = (context_->sprite[entry].attr & 0x20) == 0;
            if (fg_pix != 0) {
              if (entry == 0) sprite_zero_rendered_ = true;
              break;
            }
          }
        }
      }
      Byte pix = 0x00;
      Byte pal = 0x00;
      if (bg_pix == 0 && fg_pix == 0) {
        pix = 0x00;
        pal = 0x00;
      } else if (bg_pix == 0 && fg_pix > 0) {
        pix = fg_pix;
        pal = fg_pal;
      } else if (bg_pix > 0 && fg_pix == 0) {
        pix = bg_pix;
        pal = bg_pal;
      } else if (bg_pix > 0 && fg_pix > 0) {
        pix = fg_pri ? fg_pix : bg_pix;
        pal = fg_pri ? fg_pal : bg_pal;
        if (SpriteZeroHitOccur()) SpriteZeroHitAt(cycle);
      }
      if (0 <= cycle - 1 && cycle -1 < PPU::kFrameW && 0 <= scanline && scanline < PPU::kFrameH)
        context_->pixel_writer(
          cycle - 1,
          scanline,
          colours_->Get(BIT(ppumask, intensity), mmu_->Read(0x3F00 + (pal << 2) + pix) & 0x3F));
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    bool Is8x8Mode() const {
      return !BIT(ppuctrl, sprite_height);
    }

    bool IsTopHalf(std::int16_t scanline, std::size_t entry) const {
      return scanline - context_->sprite[entry].y < 8;
    }

    bool IsFlippedV(std::size_t entry) const {
      NESDEV_CORE_CASSERT(entry < PPU::kNumSprites + 1, "Invalid sprite entry specified");
      return context_->sprite[entry].attr & 0x80;
    }

    bool IsFlippedH(std::size_t entry) const {
      NESDEV_CORE_CASSERT(entry < PPU::kNumSprites + 1, "Invalid sprite entry specified");
      return context_->sprite[entry].attr & 0x40;
    }

    bool SpriteZeroHitOccur() {
      return may_sprite_zero_hit_
        && sprite_zero_rendered_
        && BIT(ppumask, background_enable)
        && BIT(ppumask, sprite_enable);
    }

    void SpriteZeroHitAt(std::int16_t cycle) {
      if (!(BIT(ppumask, background_leftmost_enable) || BIT(ppumask, sprite_leftmost_enable))) {
        if (cycle >= 9 && cycle < 258) REG(ppustatus) |= MSK(sprite_zero_hit);
      } else {
        if (cycle >= 1 && cycle < 258) REG(ppustatus) |= MSK(sprite_zero_hit);
      }
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    PPU::Context* context_;

    PPU::Colours* colours_;

    PPU::Registers* const registers_;

    PPU::Shifters* const shifters_;

    MMU* const mmu_;

    PPU::Chips* const chips_;

    bool may_sprite_zero_hit_  = false;

    bool sprite_zero_rendered_ = false;
  };

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  static MemoryMap Map(Address address) {
    switch (address % 0x08) {
    case 0x0000: return MemoryMap::PPUCTRL;
    case 0x0001: return MemoryMap::PPUMASK;
    case 0x0002: return MemoryMap::PPUSTATUS;
    case 0x0003: return MemoryMap::OAMADDR;
    case 0x0004: return MemoryMap::OAMDATA;
    case 0x0005: return MemoryMap::PPUSCROLL;
    case 0x0006: return MemoryMap::PPUADDR;
    case 0x0007: return MemoryMap::PPUDATA;
    default: NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::RP2C02::Map", address));
    }
  }

  [[nodiscard]]
  bool IsMaster() const {
    return BIT(ppuctrl, ppu_master_slave);
  }

  [[nodiscard]]
  bool IsNMIEnable() const {
    return BIT(ppuctrl, nmi_enable);
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  /* [SEE] https://wiki.nesdev.com/w/index.php/PPU_rendering */
  void Ticked() {
    NextCycle();
    if (IsRendering() && (Cycle() == 260 && Scanline() < 240))
      rom_->mapper->OnVisibleCycleEnds();
    if (Cycle() >= 341) {
      Cycle(0); NextScanline();
      if (Scanline() >= 261) {
        Scanline(-1); TransitFrame();
      }
    }
  }

  [[nodiscard]]
  Byte Latched() const {
    return latch_.Latched();
  }

  void ReadPPUCtrl() {
    latch_.ReadPPUCtrl();
  }

  void ReadPPUMask() {
    latch_.ReadPPUMask();
  }
    
  void ReadPPUStatus() {
    latch_.ReadPPUStatus();
  }

  void ReadOAMAddr() {
    latch_.ReadOAMAddr();
  }

  void ReadOAMData() {
    latch_.ReadOAMData();
  }

  void ReadPPUScroll() {
    latch_.ReadPPUScroll();
  }

  void ReadPPUAddr() {
    latch_.ReadPPUAddr();
  }

  void ReadPPUData() {
    latch_.ReadPPUData();
  }

  void WritePPUCtrl(Byte byte) {
    latch_.WritePPUCtrl(byte);
  }

  void WritePPUMask(Byte byte) {
    latch_.WritePPUMask(byte);
  }

  void WritePPUStatus(Byte byte) {
    latch_.WritePPUStatus(byte);
  }

  void WriteOAMAddr(Byte byte) {
    latch_.WriteOAMAddr(byte);
  }

  void WriteOAMData(Byte byte) {
    latch_.WriteOAMData(byte);
  }

  void WritePPUScroll(Byte byte) {
    latch_.WritePPUScroll(byte);
  }

  void WritePPUAddr(Byte byte) {
    latch_.WritePPUAddr(byte);
  }

  void WritePPUData(Byte byte) {
    latch_.WritePPUData(byte);
  }

  void UpdateShiftAt(std::int16_t cycle) {
    shift_.UpdateAt(cycle);
  }

  void LoadBg() {
    shift_.LoadBg();
  }

  void ReadBgId() {
    shift_.ReadBgId();
  }

  void ReadBgAttr() {
    shift_.ReadBgAttr();
  }

  void ReadBgLSB() {
    shift_.ReadBgLSB();
  }

  void ReadBgMSB() {
    shift_.ReadBgMSB();
  }

  void ClearSp() {
    shift_.ClearSp();
  }

  void EvaluateSpAt(std::int16_t scanline) {
    shift_.EvaluateSpAt(scanline);
  }

  void GatherSpAt(std::int16_t scanline) {
    shift_.GatherSpAt(scanline);
  }

  void ComposeAt(std::int16_t cycle, std::int16_t scanline) {
    shift_.ComposeAt(cycle, scanline);
  }

  void ScrollX() {
    if (IsRendering()) {
      // A single name table is 32 x 30 tiles.
      if (BIT(vramaddr, coarse_x) == 31) {
        BIT(vramaddr, coarse_x)    = 0;
        BIT(vramaddr, nametable_x) = ~BIT(vramaddr, nametable_x);
      } else {
        BIT(vramaddr, coarse_x)++;
      }
    }
  }

  void ScrollY() {
    if (IsRendering()) {
      // A single name table is 32 x 30 tiles.                
      // According to the existence of attribute memory, an increment in Yx first adjusts the fine offset.
      if (BIT(vramaddr, fine_y) < 7) {
        BIT(vramaddr, fine_y)++;
      } else {
        BIT(vramaddr, fine_y) = 0;
        if (BIT(vramaddr, coarse_y) == 29) {
          BIT(vramaddr, coarse_y)    = 0;
          BIT(vramaddr, nametable_y) = ~BIT(vramaddr, nametable_y);
        } else if (BIT(vramaddr, coarse_y) == 31) {
          BIT(vramaddr, coarse_y) = 0;
        } else {
          BIT(vramaddr, coarse_y)++;
        }
      }
    }
  }

  void TransferX() {
    if (IsRendering()) {
      BIT(vramaddr, nametable_x) = unsigned(BIT(tramaddr, nametable_x));
      BIT(vramaddr, coarse_x)    = unsigned(BIT(tramaddr, coarse_x));
    }
  }

  void TransferY() {
    if (IsRendering()) {
      BIT(vramaddr, fine_y)      = unsigned(BIT(tramaddr, fine_y));
      BIT(vramaddr, nametable_y) = unsigned(BIT(tramaddr, nametable_y));
      BIT(vramaddr, coarse_y)    = unsigned(BIT(tramaddr, coarse_y));
    }
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  PPU::Chips* const chips_;

  PPU::Registers* const registers_;

  PPU::Shifters* const shifters_;

  MMU* const mmu_;

  Latch latch_;

  Shift shift_;
};

#undef REG
#undef BIT
#undef MSK
#undef BACK
#undef SHIFT_BACK
#undef PUSH_BACK
#undef SPRT
#undef SHIFT_SPRT
#undef PUSH_SPRT
#undef FINE_X

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_RP2C02_H_
