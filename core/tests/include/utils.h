/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TEST_UTILS_H_
#define _NESDEV_CORE_TEST_UTILS_H_
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {

#define DBG(...) \
  do { \
    ColoredPrintf(testing::internal::COLOR_YELLOW, "[ DEBUG    ] "); \
    ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); \
  } while(0)

class DebugOut : public std::stringstream {
 public:
  ~DebugOut() {
    DBG("%s", str().c_str());
  }
};

#define COUT DebugOut()

class Utility {
 public:
  static void Init() noexcept {
    srand(time(nullptr));
  }

  template<Address From, Address To>
  static Address RandomAddress() noexcept {
    return From + rand() % (To - From + 1);
  }

  template<Byte From, Byte To>
  static Byte RandomByte() noexcept {
    return From + rand() % (To - From + 1);
  }

 private:
  Utility();
};

}  // namespace core
}  // namespace nesdev

#endif  // ifndef _NESDEV_CORE_TEST_UTILS_H_
