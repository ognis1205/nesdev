/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PPU_H_
#define _NESDEV_CORE_PPU_H_
#include "nesdev/core/clock.h"
//#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class PPU : public Clock {
 public:
  virtual ~PPU() = default;

  virtual void Tick() override = 0;

 protected:
  struct Context {
    void Clear() {
    }
  };

 protected:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_H_
