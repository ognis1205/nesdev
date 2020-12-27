/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_IMPL_MEMORY_BANK_H_
#define _NES_CORE_IMPL_MEMORY_BANK_H_
#include "nes/core/memory_bank.h"
#include "nes/core/types.h"

namespace nes {
namespace core {
namespace impl {

template <Address From, Address To, Address Size>
class MemoryBank final : public nes::core::MemoryBank {
 public:
  static_assert(Size > 0u, "Size must be greater than zero.");
  static_assert(From <= To, "Start address must be greater than end address.");
  static_assert((To - From + 1u) % Size == 0, "Size does not match address range.");

  MemoryBank() = default;

  [[nodiscard]]
  bool IsAddressInRange(const Address& address) const override;

  Byte Read(const Address& address) const override;

  void Write(const Address& address, const Byte& byte) override;

 private:
  Byte* GetLocation(const Address& address);

  const Byte* GetLocation(const Address& address) const;

  std::array<Byte, Size> data_ {};
};

}  // namespace impl
}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_IMPL_MEMORY_BANK_H_
