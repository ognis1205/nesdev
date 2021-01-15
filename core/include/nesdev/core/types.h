/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TYPES_H_
#define _NESDEV_CORE_TYPES_H_
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace nesdev {
namespace core {

using Byte    = std::uint8_t;

using Word    = std::uint16_t;

using Address = std::uint16_t;

template <Address Start, Address End>
class AddressSpace {
 public:
  static_assert(
    Start <= End,
    "Start address must be greater than end address");

  static const Address LBound = Start;

  static const Address RBound = End;

 public:
  bool Contain(Address address) const noexcept {
    if constexpr (Start == 0) return address <= End;
    else return address >= Start && address <= End;
  }
};

template <size_t BitNo, size_t Width = 1, typename T = Byte>
struct Bitfield {
  static constexpr auto mask = ((1u << Width) - 1u) << BitNo;

  template <typename U>
  Bitfield& operator=(U that) {
    value_ = (value_ & ~mask) |
             ((Width > 1 ? that & (mask >> BitNo) : !!that) << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator|=(U that) {
    unsigned value = *this;
    value_ =
        (value_ & ~mask) |
        ((Width > 1 ? (value | (that & (mask >> BitNo))) : (value | !!that))
         << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator&=(U that) {
    unsigned value = *this;
    value_ =
        (value_ & ~mask) |
        ((Width > 1 ? (value & (that & (mask >> BitNo))) : (value & !!that))
         << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator^=(U that) {
    unsigned value = *this;
    value_ =
        (value_ & ~mask) |
        ((Width > 1 ? (value ^ (that & (mask >> BitNo))) : (value ^ !!that))
         << BitNo);
    return *this;
  }

  operator unsigned() const { return (value_ & mask) >> BitNo; }

  Bitfield& operator++() { return *this = *this + 1; }

  unsigned operator++(int) {
    unsigned ret = *this;
    ++*this;
    return ret;
  }

  Bitfield& operator--() { return *this = *this - 1; }

  unsigned operator--(int) {
    unsigned ret = *this;
    --*this;
    return ret;
  }

  T value_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TYPES_H_
