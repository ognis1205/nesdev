/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_NES_H_
#define _NESDEV_CORE_NES_H_
#include <memory.h>
#include "nesdev/core/clock.h"
#include "nesdev/core/cpu.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class NES final : public Clock {
 public:
  class DirectMemoryAccess {
   public:
    virtual Byte Read([[maybe_unused]] Address address) {
      return address_.page;
    }

    void Write([[maybe_unused]] Address address, Byte byte) {
      address_.page   = byte;
      address_.offset = 0x00;
      transfer_       = true;
    }

    void Load(MMU* const cpu_bus) {
      data_ = cpu_bus->Read(address_.value);
    }

    void Transfer(PPU::Chips* const ppu_chips) {
      ppu_chips->oam->Write(address_.offset++, data_);
      if (address_.offset == 0x00) {
	transfer_            = false;
	wait_for_even_cycle_ = true;
      }
    }

    bool IsTransfering() {
      return transfer_;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    union {
      Address value;
      Bitfield<0, 8, Address> offset;
      Bitfield<8, 8, Address> page;
    } address_ = {0x0000};

    Byte data_ = {0x00};

    bool transfer_ = false;

    bool wait_for_even_cycle_ = true;
  };

 public:
  NES(std::unique_ptr<ROM> rom);

  ~NES() = default;

  virtual void Tick() override;

 public:
  const std::unique_ptr<ROM> rom;

  const std::unique_ptr<DirectMemoryAccess> dma;

  const std::unique_ptr<PPU::Registers> ppu_registers;

  const std::unique_ptr<PPU::Shifters> ppu_shifters;

  const std::unique_ptr<PPU::Chips> ppu_chips;

  const std::unique_ptr<MMU> ppu_bus;

  const std::unique_ptr<PPU> ppu;

  const std::unique_ptr<CPU::Registers> cpu_registers;

  const std::unique_ptr<MMU> cpu_bus;

  const std::unique_ptr<CPU> cpu;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_NES_H_
