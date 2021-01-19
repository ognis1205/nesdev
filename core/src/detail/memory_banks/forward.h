/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_FORWARD_H_
#define _NESDEV_CORE_DETAIL_MEMORY_BANKS_FORWARD_H_
#include <cstddef>
#include <functional>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {
namespace detail {
namespace memory_banks {

template <Address From, Address To>
class Forward final : public MemoryBank {
 public:
  using Reader = std::function<Byte(Address)>;

  using Writer = std::function<void(Address, Byte)>;

 public:
  static_assert(
    From <= To,
    "Start address must be greater than end address");

 public:
  Forward(std::size_t size, Reader reader, Writer writer)
    : size_{size},
      reader_{std::move(reader)},
      writer_{std::move(writer)} {
    NESDEV_CORE_CASSERT((To - From + 1u) % size == 0, "Size does not match address range");
  }

  [[nodiscard]]
  bool HasValidAddress([[maybe_unused]] Address address) const noexcept override {
    if constexpr (From == 0) return address <= To;
    else return address >= From && address <= To;
  }

  Byte Read(Address address) const override {
    if (HasValidAddress(address)) return reader_(address);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Read", address));
  }

  void Write(Address address, [[maybe_unused]] Byte byte) override {
    if (HasValidAddress(address)) writer_(address, byte);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to Write", address));
  }

  std::size_t Size() const override {
    return size_;
  }

  Byte* Data() override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to memory forwarding"));
  }

  const Byte* Data() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to memory forwarding"));
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  std::size_t size_;

  Reader reader_;

  Writer writer_;
};

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_FORWARD_H_
