/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MMU_H_
#define _NESDEV_CORE_MMU_H_
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class MMU {
 public:
  virtual ~MMU() = default;

  virtual void Set(MemoryBanks memory_banks) NESDEV_CORE_NOEXCEPT = 0;

  virtual Byte Read(const Address& address) NESDEV_CORE_CONST = 0;

  virtual void Write(const Address& address, const Byte& byte) = 0;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MMU_H_
