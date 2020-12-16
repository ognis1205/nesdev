/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstdint>
#include <cstdlib>

namespace nes {
namespace types {

using size_t = std::size_t;
using int8_t = std::int_least8_t;
using uint8_t = std::uint_least8_t;
using uint16_t = std::uint_least16_t;
using uint32_t = std::uint_least32_t;

template <size_t BitNo, size_t Width = 1, typename T = uint8_t>
struct Bitfield {
  static constexpr auto kMask = (1u << Width) - 1u;
  template <typename U>
  Bitfield& operator=(U that) {
    register_ = (register_ & ~(kMask << BitNo)) |
                ((Width > 1 ? that & kMask : !!that) << BitNo);
    return *this;
  }
  operator unsigned() const { return (register_ >> BitNo) & kMask; }
  Bitfield& operator++() { return *this = *this + 1; }
  unsigned operator++(int) {
    unsigned ret = *this;
    ++*this;
    return ret;
  }
  T register_;
};

}  // namespace types
}  // namespace nes
