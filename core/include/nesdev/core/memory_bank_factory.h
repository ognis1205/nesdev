/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MEMORY_BANK_FACTORY_H_
#define _NESDEV_CORE_MEMORY_BANK_FACTORY_H_
#include <memory>
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/rom.h"

namespace nesdev {
namespace core {

class MemoryBankFactory {
 public:
  [[nodiscard]]
  static MemoryBanks CPUBus(ROM* const rom, PPU* const ppu);

  [[nodiscard]]
  static MemoryBanks PPUBus(ROM* const rom);
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MEMORY_BANK_FACTORY_H_
