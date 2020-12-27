/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_DETAIL_MEMORY_BANK_H_
#define _NES_CORE_DETAIL_MEMORY_BANK_H_
#include "nes/core/memory_bank.h"
#include "nes/core/types.h"
#include "macros.h"

namespace nes {
namespace core {
namespace detail {

template <Address From, Address To, Address Size>
class MemoryBank final : public nes::core::MemoryBank {
 public:
  static_assert(Size > 0u, "Size must be greater than zero.");
  static_assert(From <= To, "Start address must be greater than end address.");
  static_assert((To - From + 1u) % Size == 0, "Size does not match address range.");

  MemoryBank() = default;

  [[nodiscard]]
  bool IsAddressInRange(const Address& address) const override {
    if constexpr (From == 0) {
      return address <= To;
    } else {
      return address >= From && address <= To;
    }
  }

  Byte Read(const Address& address) const override {
    return *GetLocation(address);
  }

  void Write(const Address& address, const Byte& byte) override {
    *GetLocation(address) = byte;
  }

 NES_CORE_PRIVATE_UNLESS_TESTED:
  Byte* GetLocation(const Address& address) {
    return const_cast<Byte*>(std::as_const(*this).GetLocation(address));
  }

  const Byte* GetLocation(const Address& address) const {
    return &bank_.data()[address % Size];
  }

  std::array<Byte, Size> bank_{};
};

}  // namespace detail
}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_DETAIL_MEMORY_BANK_H_
