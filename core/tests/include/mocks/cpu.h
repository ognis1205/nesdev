/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_CPU_H_
#define _NESDEV_CORE_MOCKS_CPU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class CPU : public nesdev::core::CPU {
 public:
  MOCK_METHOD0(Tick, void());

  MOCK_METHOD0(Next, void());

  MOCK_CONST_METHOD0(MockIsIdle, bool());

  MOCK_METHOD0(MockFetch, Byte());

  MOCK_METHOD0(MockReset, void());

  MOCK_METHOD0(MockIRQ, void());

  MOCK_METHOD0(MockNMI, void());

  MOCK_CONST_METHOD0(MockPCRegister, Byte());

  MOCK_CONST_METHOD0(MockARegister, Byte());

  MOCK_CONST_METHOD0(MockXRegister, Byte());

  MOCK_CONST_METHOD0(MockYRegister, Byte());

  MOCK_CONST_METHOD0(MockSRegister, Byte());

  MOCK_CONST_METHOD0(MockPRegister, Byte());

  bool IsIdle() const noexcept {
    return MockIsIdle();
  }

  Byte Fetch() noexcept {
    return MockFetch();
  }

  void Reset() noexcept {
    MockReset();
  }

  void IRQ() noexcept {
    MockIRQ();
  }

  void NMI() noexcept {
    MockNMI();
  }

  Byte PCRegister() const noexcept {
    return MockPCRegister();
  }

  Byte ARegister() const noexcept {
    return MockARegister();
  }

  Byte XRegister() const noexcept {
    return MockXRegister();
  }

  Byte YRegister() const noexcept {
    return MockYRegister();
  }

  Byte SRegister() const noexcept {
    return MockSRegister();
  }

  Byte PRegister() const noexcept {
    return MockPRegister();
  }
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_CPU_H_
