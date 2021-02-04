/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_MEMORY_BANK_H_
#define _NESDEV_CORE_MOCKS_MEMORY_BANK_H_
#include <cstddef>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class MemoryBank : public nesdev::core::MemoryBank {
 public:
  MOCK_CONST_METHOD1(HasValidAddress, bool(nesdev::core::Address));

  MOCK_CONST_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));

  MOCK_CONST_METHOD0(Size, std::size_t());

  MOCK_METHOD0(Data, Byte*());

  MOCK_CONST_METHOD0(Data, const Byte*());
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_MEMORY_BANK_H_
