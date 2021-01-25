/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include "nesdev/core/ppu.h"
#include "nesdev/core/ppu_factory.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/palettes.h"
#include "detail/rp2c02.h"

namespace nesdev {
namespace core {

std::unique_ptr<PPU> PPUFactory::RP2C02(PPU::Chips* const chips,
					PPU::Registers* const registers,
                                        PPU::Shifters* const shifters,
                                        MMU* const mmu) {
  return std::make_unique<detail::RP2C02>(
    chips,
    registers,
    shifters,
    mmu,
    Palettes::RP2C02());
}

}  // namespace core
}  // namespace nesdev
