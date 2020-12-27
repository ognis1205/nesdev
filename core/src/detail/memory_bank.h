/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_DETAIL_MEMORY_BANK_H_
#define _NES_CORE_DETAIL_MEMORY_BANK_H_
#include "nes/core/exceptions.h"
#include "nes/core/memory_bank.h"
#include "nes/core/types.h"
#include "macros.h"

namespace nes {
namespace core {
namespace detail {

template <Address From, Address To, Address Range>
class MemoryBank final : public nes::core::MemoryBank {
 public:
  static_assert(
    Range > 0u,
    "[nes::core::detail::MemoryBank] Range must be greater than zero.");
  static_assert(
    From <= To,
    "[nes::core::detail::MemoryBank] Start address must be greater than end address.");
  static_assert(
    (To - From + 1u) % Range == 0,
    "[nes::core::detail::MemoryBank] Range does not match address range.");

  MemoryBank() = default;

  [[nodiscard]]
  bool HasValidAddress(const Address& address) const override {
    if constexpr (From == 0) {
      return address <= To;
    } else {
      return address >= From && address <= To;
    }
  }

  Byte Read(const Address& address) const override {
    if (HasValidAddress(address)) return *PointerTo(address);
    else NES_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Read."));
  }

  void Write(const Address& address, const Byte& byte) override {
    if (HasValidAddress(address)) *PointerTo(address) = byte;
    else NES_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Write."));
  }

 NES_CORE_PRIVATE_UNLESS_TESTED:
  Byte* PointerTo(const Address& address) {
    return const_cast<Byte*>(std::as_const(*this).PointerTo(address));
  }

  const Byte* PointerTo(const Address& address) const {
    return &bank_.data()[address % Range];
  }

  std::array<Byte, Range> bank_{};
};

}  // namespace detail
}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_DETAIL_MEMORY_BANK_H_
