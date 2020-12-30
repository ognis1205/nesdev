/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MMU_FACTORY_H_
#define _NESDEV_CORE_MMU_FACTORY_H_
#include <memory>
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/mmu.h"

namespace nesdev {
namespace core {

class MMUFactory {
 public:
  [[nodiscard]]
  static std::unique_ptr<MMU> Create();

  [[nodiscard]]
  static std::unique_ptr<MMU> Create(MemoryBanks memory_banks);
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MMU_FACTORY_H_
