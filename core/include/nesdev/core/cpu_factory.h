/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CPU_FACTORY_H_
#define _NESDEV_CORE_CPU_FACTORY_H_
#include <memory>
#include "nesdev/core/cpu.h"
#include "nesdev/core/mmu.h"

namespace nesdev {
namespace core {

class CPUFactory {
 public:
  [[nodiscard]]
  static std::unique_ptr<CPU> RP2A03(CPU::Registers* const registers, MMU* const mmu);
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_FACTORY_H_
