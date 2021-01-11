/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TEST_MOCK_MMU_H_
#define _NESDEV_CORE_TEST_MOCK_MMU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace test {

class MockMMU : public MMU {
 public:
  MOCK_METHOD1(MockSet, void(MemoryBanks));

  MOCK_CONST_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));

  virtual void Set(MemoryBanks memory_banks) noexcept {
    return MockSet(std::move(memory_banks));
  }
};

}  // namespace test
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TEST_MOCK_MMU_H_