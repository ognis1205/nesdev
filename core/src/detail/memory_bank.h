/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MEMORY_BANK_H_
#define _NESDEV_CORE_DETAIL_MEMORY_BANK_H_
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {

template <Address From, Address To, Address Range>
class MemoryBank final : public nesdev::core::MemoryBank {
 public:
  static_assert(
    Range > 0u,
    "[nesdev::core::detail::MemoryBank] Range must be greater than zero");
  static_assert(
    From <= To,
    "[nesdev::core::detail::MemoryBank] Start address must be greater than end address");
  static_assert(
    (To - From + 1u) % Range == 0,
    "[nesdev::core::detail::MemoryBank] Range does not match address range");

  MemoryBank() = default;

  [[nodiscard]]
  bool HasValidAddress(const Address& address) NESDEV_CORE_CONST NESDEV_CORE_NOEXCEPT override {
    if constexpr (From == 0) return address <= To;
    else return address >= From && address <= To;
  }

  Byte Read(const Address& address) NESDEV_CORE_CONST override {
    if (HasValidAddress(address)) return *PointerTo(address);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Read", address));
  }

  void Write(const Address& address, const Byte& byte) override {
    if (HasValidAddress(address)) *PointerTo(address) = byte;
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Write", address));
  }

 NESDEV_CORE_PRIVATE:
  Byte* PointerTo(const Address& address) {
    return const_cast<Byte*>(std::as_const(*this).PointerTo(address));
  }

  const Byte* PointerTo(const Address& address) const {
    return &bank_.data()[address % Range];
  }

  std::array<Byte, Range> bank_ = {};
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MEMORY_BANK_H_
