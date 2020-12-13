/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstddef>
#include <cstdint>

namespace nes {
namespace types {

/* Type Name Aliases. */
using s8_t = std::int_least8_t;
using u8_t = std::uint_least8_t;
using u32_t = std::uint_least32_t;
using u16_t = std::uint_least16_t;

/* Register Bitfield Accessor Class. */
template <size_t BitNo, size_t Width = 1, typename T = u8_t>
class RegisterBits {
 public:
  template <typename U>
  RegisterBits& operator=(U rhs) {
    register_ = (register_ & ~(kMask_ << BitNo)) |
                ((Width > 1 ? rhs & kMask_ : !!rhs) << BitNo);
    return *this;
  }
  RegisterBits& operator++() { return *this = *this + 1; }
  unsigned operator++(int) { unsigned ret = *this; ++*this; return ret; }
  operator unsigned() const { return (register_ >> BitNo) & kMask_; }

 private:
  static constexpr auto kMask_ = (1u << Width) - 1u;
  T register_;
};

}  // namespace types
}  // namespace nes
