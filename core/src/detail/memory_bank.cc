/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
//#include "nes/core/memory_bank.h"
#include "nes/core/types.h"
#include "detail/memory_bank.h"

namespace nes {
namespace core {
namespace detail {

template <Address From, Address To, Address Size>
bool MemoryBank<From, To, Size>::IsAddressInRange(const Address &address) const {
  if constexpr (From == 0) {
    return address <= To;
  } else {
    return address >= From && address <= To;
  }
}

template <Address From, Address To, Address Size>
Byte MemoryBank<From, To, Size>::Read(const Address &address) const {
  return *GetLocation(address);
}

template <Address From, Address To, Address Size>
void MemoryBank<From, To, Size>::Write(const Address& address, const Byte& byte) {
  *GetLocation(address) = byte;
}

template <Address From, Address To, Address Size>
Byte* MemoryBank<From, To, Size>::GetLocation(const Address& address) {
  return const_cast<Byte*>(std::as_const(*this).GetLocation(address));
}

template <Address From, Address To, Address Size>
const Byte* MemoryBank<From, To, Size>::GetLocation(const Address& address) const {
  return &bank_.data()[address % Size];
}

}  // namespace detail
}  // namespace core
}  // namespace nes
