/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TEST_MOCK_CPU_H_
#define _NESDEV_CORE_TEST_MOCK_CPU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class CPU : public nesdev::core::CPU {
 public:
  MOCK_METHOD0(Tick, void());

  MOCK_METHOD0(Next, bool());

  MOCK_METHOD0(MockFetch, Byte());

  MOCK_METHOD0(MockRST, bool());

  MOCK_METHOD0(MockIRQ, bool());

  MOCK_METHOD0(MockNMI, bool());

  Byte Fetch() noexcept {
    return MockFetch();
  }

  bool RST() noexcept {
    return MockRST();
  }

  bool IRQ() noexcept {
    return MockIRQ();
  }

  bool NMI() noexcept {
    return MockNMI();
  }
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TEST_MOCK_CPU_H_
