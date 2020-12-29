/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MMU_H_
#define _NESDEV_CORE_DETAIL_MMU_H_
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/types.h"
#include "macros.h"

namespace nesdev {
namespace core {
namespace detail {

class MMU final : public nesdev::core::MMU {
 public:
  void Clear() noexcept;

  void Add(std::unique_ptr<MemoryBank> memory_bank);

  void Set(MemoryBanks memory_banks) noexcept override;

  Byte Read(const Address& address) const override;

  void Write(const Address& address, const Byte& byte) override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  MemoryBank* Switch(const Address& address) const;

  MemoryBanks memory_banks_ = {};
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MMU_H_
