/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_CONNECTOR_H_
#define _NESDEV_CORE_DETAIL_MEMORY_BANKS_CONNECTOR_H_
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
class Connector final : public MemoryBank {
 public:
  using Reader = std::function<Byte(Address)>;

  using Writer = std::function<void(Address, Byte)>;

 public:
  static_assert(
    From <= To,
    "Start address must be greater than end address");

 public:
  Connector(Reader reader, Writer writer)
    : reader_{std::move(reader)},
      writer_{std::move(writer)} {}

  [[nodiscard]]
  bool HasValidAddress([[maybe_unused]] Address address) const override {
    if constexpr (From == 0) return address <= To;
    else return address >= From && address <= To;
  }

  Byte Read(Address address) const override {
    if (HasValidAddress(address)) return reader_(address);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::memory_banks::Connector::Read", address));
  }

  void Write(Address address, [[maybe_unused]] Byte byte) override {
    if (HasValidAddress(address)) writer_(address, byte);
    else NESDEV_CORE_THROW(InvalidAddress::Occur("Invalid address specified to nesdev::core::detail::memory_banks::Connector::Write", address));
  }

  std::size_t Size() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to nesdev::core::detail::memory_banks::Connector"));
  }

  Byte* Data() override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to nesdev::core::detail::memory_banks::Connector"));
  }

  const Byte* Data() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to nesdev::core::detail::memory_banks::Connector"));
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Reader reader_;

  Writer writer_;
};

}  // namespace memory_banks
}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_MEMORY_BANKS_CONNECTOR_H_
