/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>
#include "utils.h"

namespace nesdev {
namespace core {

class OpcodesTest : public testing::Test {
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
};

TEST_F(OpcodesTest, Decode) {
  for (auto i = 0x00u; i <= 0xFFu; i++) {
    EXPECT_NO_THROW({
	auto opcode = Opcodes::Decode(i);
	static_cast<void>(opcode);
      });
  }
}

TEST_F(OpcodesTest, ToString) {
  for (auto i = 0x00u; i <= 0xFFu; i++) {
    EXPECT_NE("UNKNOWN, UNKNOWN", Opcodes::ToString(i));
  }
}

}  // namespace core
}  // namespace nesdev
