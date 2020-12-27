/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_MEMORY_BANK_H_
#define _NES_CORE_MEMORY_BANK_H_
#include <memory>
#include <vector>
#include "nes/core/types.h"

namespace nes {
namespace core {

class MemoryBank {
 public:
  virtual ~MemoryBank() = default;

  [[nodiscard]]
  virtual bool IsAddressInRange(const Address& address) const = 0;

  virtual Byte Read(const Address& address) const = 0;

  virtual void Write(const Address& address, const Byte& byte) = 0;
};

using MemoryBankList = std::vector<std::unique_ptr<MemoryBank>>;

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_MEMORY_BANK_H_
