/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_MMU_H_
#define _NESDEV_CORE_MOCKS_MMU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class MMU : public nesdev::core::MMU {
 public:
  MOCK_METHOD1(Set, void(MemoryBanks));

  MOCK_CONST_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_MMU_H_
