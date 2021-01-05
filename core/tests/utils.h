/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TESTS_UTILS_H_
#define _NESDEV_CORE_TESTS_UTILS_H_
#include <bitset>
#include <iomanip>
#include <sstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

#endif  // ifndef _NESDEV_CORE_TESTS_UTILS_H_
