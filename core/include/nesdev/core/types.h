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
#include "nesdev/core/macros.h"

namespace nesdev {
namespace core {

using Byte = std::uint_least8_t;

using Address = std::uint_least16_t;

template <size_t BitNo, size_t Width = 1, typename T = Byte>
struct Bitfield {
  static constexpr auto kMask = (1u << Width) - 1u;

  template <typename U>
  Bitfield& operator=(U that) {
    value_ = (value_ & ~(kMask << BitNo)) |
            ((Width > 1 ? that & kMask : !!that) << BitNo);
    return *this;
  }

  operator unsigned() const { return (value_ >> BitNo) & kMask; }

  Bitfield& operator++() { return *this = *this + 1; }

  unsigned operator++(int) {
    unsigned ret = *this;
    ++*this;
    return ret;
  }

  T value_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TYPES_H_
