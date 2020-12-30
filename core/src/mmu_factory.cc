/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/mmu_factory.h"
#include "detail/mmu.h"

namespace nesdev {
namespace core {

std::unique_ptr<MMU> MMUFactory::Create() {
  return std::make_unique<detail::MMU>();
}

std::unique_ptr<MMU> MMUFactory::Create(MemoryBanks memory_banks) {
  std::unique_ptr<MMU> mmu_ptr = std::make_unique<detail::MMU>();
  auto mmu = static_cast<detail::MMU*>(mmu_ptr.get());
  for (std::unique_ptr<MemoryBank>& memory_bank : memory_banks)
    mmu->Add(std::move(memory_bank));
  return mmu_ptr;
}

}  // namespace core
}  // namespace nesdev
