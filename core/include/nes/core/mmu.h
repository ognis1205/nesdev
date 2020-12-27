/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_MMU_H_
#define _NES_CORE_MMU_H_
#include "nes/core/memory_bank.h"
#include "nes/core/types.h"

namespace nes {
namespace core {

class MMU {
 public:
  virtual ~MMU() = default;

  virtual void Set(MemoryBankList memory_banks) = 0;

  virtual Byte Read(const Address& address) const = 0;

  virtual void Write(const Address& address, const Byte& byte) = 0;
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_MMU_H_
