/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <memory>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "detail/mmu.h"
#include "utils.h"
#include "mocks/cpu.h"

namespace nesdev {
namespace core {

class CPUTest : public testing::Test {
 protected:
  void SetUp() override {
    Utility::Init();
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  mocks::CPU cpu_;
};

TEST_F(CPUTest, Context) {
  EXPECT_EQ(0, cpu_.Cycle());
  EXPECT_EQ(0x00, cpu_.Fetched());
  EXPECT_EQ(0x00, cpu_.Op());
  EXPECT_EQ(0x0000, cpu_.Addr());
  EXPECT_EQ(0x0000, cpu_.Ptr());
  EXPECT_FALSE(cpu_.CrossPage());
  EXPECT_FALSE(cpu_.IfRST());
  EXPECT_FALSE(cpu_.IfRST());
  EXPECT_FALSE(cpu_.IfNMI());
}

TEST_F(CPUTest, Addr) {
  cpu_.Addr(0x1111);
  EXPECT_EQ(0x1111, cpu_.Addr());
  EXPECT_EQ(0x11, cpu_.AddrLo());
  EXPECT_EQ(0x11, cpu_.AddrHi());

  cpu_.Addr(0x1111, static_cast<Byte>(0xFF));
  EXPECT_EQ(0x1210, cpu_.Addr());
  EXPECT_EQ(0x10, cpu_.AddrLo());
  EXPECT_EQ(0x12, cpu_.AddrHi());
  EXPECT_TRUE(cpu_.CrossPage());

  cpu_.Addr(0x1111, static_cast<Byte>(0x01));
  EXPECT_EQ(0x1112, cpu_.Addr());
  EXPECT_EQ(0x12, cpu_.AddrLo());
  EXPECT_EQ(0x11, cpu_.AddrHi());
  EXPECT_FALSE(cpu_.CrossPage());

  cpu_.Addr(0x1111);
  cpu_.AddrHi(0x12);
  cpu_.AddrLo(0x10);
  EXPECT_EQ(0x1210, cpu_.Addr());
  EXPECT_EQ(0x10, cpu_.AddrLo());
  EXPECT_EQ(0x12, cpu_.AddrHi());
  EXPECT_FALSE(cpu_.CrossPage());

  cpu_.Addr(0x1111);
  cpu_.AddrHi(0x11);
  cpu_.AddrLo(0x12);
  EXPECT_EQ(0x1112, cpu_.Addr());
  EXPECT_EQ(0x12, cpu_.AddrLo());
  EXPECT_EQ(0x11, cpu_.AddrHi());
  EXPECT_FALSE(cpu_.CrossPage());

  cpu_.Addr(0x1111, static_cast<Address>(0x1111));
  EXPECT_EQ(0x2222, cpu_.Addr());
  EXPECT_EQ(0x22, cpu_.AddrLo());
  EXPECT_EQ(0x22, cpu_.AddrHi());
  EXPECT_TRUE(cpu_.CrossPage());

  cpu_.Addr(0x1111, static_cast<Address>(0x00FF));
  EXPECT_EQ(0x1210, cpu_.Addr());
  EXPECT_EQ(0x10, cpu_.AddrLo());
  EXPECT_EQ(0x12, cpu_.AddrHi());
  EXPECT_TRUE(cpu_.CrossPage());

  cpu_.Addr(0x1111, static_cast<Address>(0x0011));
  EXPECT_EQ(0x1122, cpu_.Addr());
  EXPECT_EQ(0x22, cpu_.AddrLo());
  EXPECT_EQ(0x11, cpu_.AddrHi());
  EXPECT_FALSE(cpu_.CrossPage());
}

TEST_F(CPUTest, Ptr) {
  cpu_.Ptr(0x1111);
  EXPECT_EQ(0x1111, cpu_.Ptr());
  EXPECT_EQ(0x11, cpu_.PtrLo());
  EXPECT_EQ(0x11, cpu_.PtrHi());

  cpu_.Ptr(0x1111);
  EXPECT_EQ(0x1111, cpu_.Ptr());
  EXPECT_EQ(0x11, cpu_.PtrLo());
  EXPECT_EQ(0x11, cpu_.PtrHi());
  EXPECT_FALSE(cpu_.CrossPage());

  cpu_.Ptr(0x1111);
  cpu_.PtrHi(0x12);
  cpu_.PtrLo(0x10);
  EXPECT_EQ(0x1210, cpu_.Ptr());
  EXPECT_EQ(0x10, cpu_.PtrLo());
  EXPECT_EQ(0x12, cpu_.PtrHi());
  EXPECT_FALSE(cpu_.CrossPage());
}

}  // namespace core
}  // namespace nesdev
