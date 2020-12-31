/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <algorithm>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"
#include "macros.h"
#include "detail/mmu.h"

namespace nesdev {
namespace core {
namespace detail {

void MMU::Clear() noexcept {
  memory_banks_.clear();
}

void MMU::Add(std::unique_ptr<MemoryBank> memory_bank) {
  memory_banks_.push_back(std::move(memory_bank));
}

void MMU::Set(MemoryBanks memory_banks) noexcept {
  memory_banks_ = std::move(memory_banks);
}

Byte MMU::Read(const Address& address) const {
  if (const MemoryBank* memory_bank = Switch(address)) return memory_bank->Read(address);
  else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Read", address));
}

void MMU::Write(const Address& address, const Byte& byte) {
  if (MemoryBank* memory_bank = Switch(address)) return memory_bank->Write(address, byte);
  else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Write", address));
}

MemoryBank* MMU::Switch(const Address& address) const {
  auto it = std::find_if(
    begin(memory_banks_),
    end(memory_banks_),
    [address](const std::unique_ptr<MemoryBank> &memory_bank) { return memory_bank->HasValidAddress(address); });
  if (it != end(memory_banks_)) return (*it).get();
  else return nullptr;
}

}  // namespace detail
}  // namespace core
}  // namespace nesdev
