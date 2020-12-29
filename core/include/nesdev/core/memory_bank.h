/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MEMORY_BANK_H_
#define _NESDEV_CORE_MEMORY_BANK_H_
#include <memory>
#include <vector>
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class MemoryBank {
 public:
  virtual ~MemoryBank() = default;

  [[nodiscard]]
  virtual bool HasValidAddress(const Address& address) const noexcept = 0;

  virtual Byte Read(const Address& address) const = 0;

  virtual void Write(const Address& address, const Byte& byte) = 0;
};

using MemoryBanks = std::vector<std::unique_ptr<MemoryBank>>;

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MEMORY_BANK_H_
