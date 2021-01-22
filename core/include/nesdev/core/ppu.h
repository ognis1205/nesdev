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
    return context_.latch;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      cycle               = {0};
      latch               = {0x00};
    }

    std::size_t cycle = {0};

    Byte latch = {0x00};
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
