/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_PPU_FACTORY_H_
#define _NESDEV_CORE_PPU_FACTORY_H_
#include <memory>
#include <vector>
#include "nesdev/core/ppu.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class PPUFactory {
 public:
  [[nodiscard]]
  static std::unique_ptr<PPU> RP2C02(PPU::Chips* const chips,
				     PPU::Registers* const registers,
                                     PPU::Shifters* const shifters,
                                     MMU* const mmu);
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_PPU_FACTORY_H_
