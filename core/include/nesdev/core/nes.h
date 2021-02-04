/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_NES_H_
#define _NESDEV_CORE_NES_H_
#include <iostream>
#include <cstddef>
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
    Byte Read([[maybe_unused]] Address address) {
      return address_.page;
    }

    void Write([[maybe_unused]] Address address, Byte byte) {
      address_.page   = byte;
      address_.offset = 0x00;
      transfer_       = true;
    }

    bool IsTransfering() {
      return transfer_;
    }

    void TransactAt(std::size_t cycle, MMU* const bus, PPU::Chips* const chips) {
      if (IsWaiting()) {
        if (cycle % 2 == 1) Ready();
      } else {
        if (cycle % 2 == 0) Load(bus);
        else Transfer(chips);
      }
    }

    void Reset() {
      address_.value       = {0x0000};
      data_                = {0x00};
      transfer_            = false;
      wait_for_even_cycle_ = true;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    bool IsWaiting() {
      return wait_for_even_cycle_;
    }

    void Ready() {
      wait_for_even_cycle_ = false;
    }

    void Load(MMU* const bus) {
      data_ = bus->Read(address_.value);
    }

    void Transfer(PPU::Chips* const chips) {
      chips->oam->Write(address_.offset++, data_);
      if (address_.offset == 0x00) {
        transfer_            = false;
        wait_for_even_cycle_ = true;
      }
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

  class Controller {
   public:
    explicit Controller() {};

    virtual ~Controller() = default;

    Byte Read([[maybe_unused]] Address address) {
      Byte data = (piso_ & 0x80) > 0;
      piso_ <<= 1;
      return data;
    }

    void Write([[maybe_unused]] Address address, [[maybe_unused]] Byte byte) {
      piso_ = state_.value;
    }

    void Right(bool pressed) {
      state_.right = pressed;
    }

    void Left(bool pressed) {
      state_.left = pressed;
    }

    void Down(bool pressed) {
      state_.down = pressed;
    }

    void Up(bool pressed) {
      state_.up = pressed;
    }

    void Start(bool pressed) {
      state_.start = pressed;
    }

    void Select(bool pressed) {
      state_.select = pressed;
    }

    void B(bool pressed) {
      state_.b = pressed;
    }

    void A(bool pressed) {
      state_.b = pressed;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    union {
      Byte value;
      Bitfield<0, 1, Byte> right;
      Bitfield<1, 1, Byte> left;
      Bitfield<2, 1, Byte> down;
      Bitfield<3, 1, Byte> up;
      Bitfield<4, 1, Byte> start;
      Bitfield<5, 1, Byte> select;
      Bitfield<6, 1, Byte> b;
      Bitfield<7, 1, Byte> a;
    } state_ = {0x00};

    Byte piso_ = {0x00};
  };

 public:
  NES(std::unique_ptr<ROM> rom);

  ~NES() = default;

  virtual void Tick() override;

 public:

 public:
  std::size_t cycle = {0};
  
  const std::unique_ptr<ROM> rom;

  const std::unique_ptr<DirectMemoryAccess> dma;

  const std::unique_ptr<Controller> controller_1;

  const std::unique_ptr<Controller> controller_2;

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
