/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_PPU_H_
#define _NESDEV_CORE_MOCKS_PPU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class PPU : public nesdev::core::PPU {
 public:
  MOCK_METHOD0(Tick, void());

  MOCK_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_PPU_H_
