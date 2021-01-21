/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PPU_H_
#define _NESDEV_CORE_PPU_H_
#include <cstddef>
#include "nesdev/core/clock.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class PPU : public Clock {
 public:
  class Nametable final {
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

  [[nodiscard]]
  Byte Latched() const noexcept {
    return context_.latched;
  }

  [[nodiscard]]
  bool IsLatched() const noexcept {
    return context_.is_latched;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      cycle             = {0};
      latched           = {0x00};
      is_latched        = false;
      address.effective = {0x0000};
      oam.address       = {0x00};
      vram.address      = {0x0000};
    }

    std::size_t cycle = {0};

    Byte latched = {0x00};

    bool is_latched = false;

    union {
      Address effective;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } address = {0x0000};

    union {
      Byte address;
    } oam = {0x00};

    union {
      Address address;
    } vram = {0x0000};
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
