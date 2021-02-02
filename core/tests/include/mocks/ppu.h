/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MOCKS_PPU_H_
#define _NESDEV_CORE_MOCKS_PPU_H_
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace mocks {

class PPU : public nesdev::core::PPU {
 public:
  PPU() : nesdev::core::PPU{std::vector<Byte>(0x40 * 3)} {}

  MOCK_METHOD0(Tick, void());

  MOCK_METHOD1(Read, Byte(Address));

  MOCK_METHOD2(Write, void(Address, Byte));

  MOCK_CONST_METHOD0(MockIsRendering, bool());

  MOCK_CONST_METHOD0(MockCtrlRegister, Byte());

  MOCK_CONST_METHOD0(MockMaskRegister, Byte());

  MOCK_CONST_METHOD0(VRAMAddr, Address());

  MOCK_CONST_METHOD0(TRAMAddr, Address());

  MOCK_CONST_METHOD0(BgPttrLo, Address());

  MOCK_CONST_METHOD0(BgPttrHi, Address());

  MOCK_CONST_METHOD0(BgAttrLo, Address());

  MOCK_CONST_METHOD0(BgAttrHi, Address());

  bool IsRendering() const noexcept {
    return MockIsRendering();
  }

  Byte CtrlRegister() const noexcept {
    return MockCtrlRegister();
  }

  Byte MaskRegister() const noexcept {
    return MockMaskRegister();
  }
};

}  // namespace mocks
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MOCKS_PPU_H_
