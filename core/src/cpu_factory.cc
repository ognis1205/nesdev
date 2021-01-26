/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include "nesdev/core/cpu.h"
#include "nesdev/core/cpu_factory.h"
#include "nesdev/core/mmu.h"
#include "detail/rp2a03.h"

namespace nesdev {
namespace core {

std::unique_ptr<CPU> CPUFactory::RP2A03(CPU::Registers* const registers, MMU* const mmu) {
  return std::make_unique<detail::RP2A03>(registers, mmu);
}

}  // namespace core
}  // namespace nesdev
