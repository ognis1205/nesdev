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
#include "nesdev/core/macros.h"

namespace nesdev {
namespace core {

using Byte    = std::uint8_t;

using Word    = std::uint16_t;

using Address = std::uint16_t;

using RGBA    = std::uint32_t;

static constexpr std::size_t k8KByte  =  8 * 1024;

static constexpr std::size_t k16KByte = 16 * 1024;

template <size_t BitNo, size_t Width = 1, typename T = Byte>
class Bitfield {
 public:
  static constexpr auto mask = ((1u << Width) - 1u) << BitNo;

 public:
  template <typename U>
  Bitfield& operator=(U that) {
    value_ = (value_ & ~mask) | ((Width > 1 ? that & (mask >> BitNo) : !!that) << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator|=(U that) {
    unsigned value = *this;
    value_ = (value_ & ~mask) | ((Width > 1 ? (value | (that & (mask >> BitNo))) : (value | !!that)) << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator&=(U that) {
    unsigned value = *this;
    value_ = (value_ & ~mask) | ((Width > 1 ? (value & (that & (mask >> BitNo))) : (value & !!that)) << BitNo);
    return *this;
  }

  template <typename U>
  Bitfield& operator^=(U that) {
    unsigned value = *this;
    value_ = (value_ & ~mask) | ((Width > 1 ? (value ^ (that & (mask >> BitNo))) : (value ^ !!that)) << BitNo);
    return *this;
  }

  operator unsigned() const {
    return (value_ & mask) >> BitNo;
  }

  Bitfield& operator++() {
    return *this = *this + 1;
  }

  unsigned operator++(int) {
    unsigned ret = *this;
    ++*this;
    return ret;
  }

  Bitfield& operator--() {
    return *this = *this - 1;
  }

  unsigned operator--(int) {
    unsigned ret = *this;
    --*this;
    return ret;
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  T value_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TYPES_H_
