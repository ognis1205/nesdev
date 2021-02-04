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

  MOCK_CONST_METHOD0(IsIdle, bool());

  MOCK_METHOD0(Fetch, Byte());

  MOCK_METHOD0(Reset, void());

  MOCK_METHOD0(IRQ, void());

  MOCK_METHOD0(NMI, void());

  MOCK_CONST_METHOD0(PCRegister, Address());

  MOCK_CONST_METHOD0(ARegister, Byte());

  MOCK_CONST_METHOD0(XRegister, Byte());

  MOCK_CONST_METHOD0(YRegister, Byte());

  MOCK_CONST_METHOD0(SRegister, Byte());

  MOCK_CONST_METHOD0(PRegister, Byte());
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_CPU_H_
