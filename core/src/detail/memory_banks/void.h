/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_VOID_H_
#define _NESDEV_CORE_DETAIL_MEMORY_BANKS_VOID_H_
#include <cstddef>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace memory_banks {

class Void final : public MemoryBank {
 public:
  Void() = default;

  [[nodiscard]]
  bool HasValidAddress([[maybe_unused]] Address address) const override {
    return false;
  }

  Byte Read(Address address) const override {
    NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::memory_banks::Void::Read", address));
  }

  void Write(Address address, [[maybe_unused]] Byte byte) override {
    NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::memory_banks::Void::Write", address));
  }

  std::size_t Size() const override {
    return 0;
  }

  Byte* Data() override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to nesdev::core::detail::memory_banks::Void"));
  }

  const Byte* Data() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to nesdev::core::detail::memory_banks::Void"));
  }
};

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_VOID_H_
