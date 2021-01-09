/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TEST_MOCK_MEMORY_BANK_H_
#define _NESDEV_CORE_TEST_MOCK_MEMORY_BANK_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace test {

class MockMemoryBank : public MemoryBank {
 public:
  MOCK_CONST_METHOD1(MockHasValidAddress, bool(nesdev::core::Address));

  MOCK_CONST_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));

  virtual bool HasValidAddress(nesdev::core::Address address) const noexcept {
    return MockHasValidAddress(address);
  }
};

}  // namespace test
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TEST_MOCK_MEMORY_BANK_H_
