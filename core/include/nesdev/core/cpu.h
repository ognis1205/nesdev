/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CPU_H_
#define _NESDEV_CORE_CPU_H_
#include <cstdlib>
#include <optional>
#include "nesdev/core/clock.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class CPU : public Clock {
 public:
  virtual ~CPU() = default;

  virtual void Tick() override = 0;

  virtual void Fetch() = 0;

  virtual void Reset() noexcept = 0;

  virtual void IRQ() noexcept = 0;

  virtual void NMI() noexcept = 0;

 protected:
  struct Context {
    size_t cycle = 0;

    std::optional<Opcode> opcode;

    Address program_counter = {0x0000};

    union {
      Address effective;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } address {0x0000};
  };

 protected:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
