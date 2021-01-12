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
  NESDEV_CORE_SASSERT(
    Range > 0u,
    "Range must be greater than zero");
  NESDEV_CORE_SASSERT(
    From <= To,
    "Start address must be greater than end address");
  NESDEV_CORE_SASSERT(
    (To - From + 1u) % Range == 0,
    "Range does not match address range");

  MemoryBank() = default;

  [[nodiscard]]
  bool HasValidAddress(Address address) const noexcept override {
    if constexpr (From == 0) return address <= To;
    else return address >= From && address <= To;
  }

  Byte Read(Address address) const override {
    if (HasValidAddress(address)) return *PtrTo(address);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Read", address));
  }

  void Write(Address address, Byte byte) override {
    if (HasValidAddress(address)) *PtrTo(address) = byte;
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid Address specified to Write", address));
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Byte* PtrTo(Address address) {
    return const_cast<Byte*>(std::as_const(*this).PtrTo(address));
  }

  const Byte* PtrTo(Address address) const {
    return &bank_.data()[address % Range];
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  std::array<Byte, Range> bank_ = {};
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MEMORY_BANK_H_
