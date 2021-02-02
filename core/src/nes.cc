/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <iostream>
#include <memory.h>
#include "nesdev/core/clock.h"
#include "nesdev/core/cpu.h"
#include "nesdev/core/cpu_factory.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/memory_bank_factory.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/mmu_factory.h"
#include "nesdev/core/nes.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/ppu_factory.h"
#include "nesdev/core/rom.h"
#include "nesdev/core/rom_factory.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

NES::NES(std::unique_ptr<ROM> rom)
    : rom{std::move(rom)},
      dma{std::make_unique<NES::DirectMemoryAccess>()},
      controller_1{std::make_unique<NES::Controller>()},
      controller_2{std::make_unique<NES::Controller>()},
      ppu_registers{std::make_unique<PPU::Registers>()},
      ppu_shifters{std::make_unique<PPU::Shifters>()},
      ppu_chips{std::make_unique<PPU::Chips>(std::make_unique<PPU::ObjectAttributeMap<64>>())},
      ppu_bus{MMUFactory::Create(MemoryBankFactory::PPUBus(this->rom.get()))},
      ppu{PPUFactory::RP2C02(ppu_chips.get(), ppu_registers.get(), ppu_shifters.get(), ppu_bus.get())},
      cpu_registers{std::make_unique<CPU::Registers>()},
      cpu_bus{MMUFactory::Create(MemoryBankFactory::CPUBus(this->rom.get(), ppu.get(), dma.get(), controller_1.get(), controller_2.get()))},
      cpu{CPUFactory::RP2A03(cpu_registers.get(), cpu_bus.get())} {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  ppu->Connect(this->rom.get());
  cpu->Reset();
  cpu_registers->p.value = {0x34};
}

void NES::Tick() {
  ppu->Tick();
  if (cycle % 3 == 0) {
    if (dma->IsTransfering()) {
//      std::cout << "OAM" << std::endl;
      dma->TransactAt(cycle, cpu_bus.get(), ppu_chips.get());
    }
    else cpu->Tick();
  }
  if (ppu_registers->ppuctrl.nmi_enable) {
    ppu_registers->ppuctrl.nmi_enable = false;
    cpu->NMI();
  }
  if (rom->mapper->IRQ()) {
    rom->mapper->ClearIRQ();
    cpu->IRQ();
  }
  cycle++;
}

}  // namespace core
}  // namespace nesdev
