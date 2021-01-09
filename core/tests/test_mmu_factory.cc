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
#include "mock_memory_bank.h"
#include "utils.h"

namespace nesdev {
namespace core {
namespace test {

class MMUFactoryTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;
};

TEST_F(MMUFactoryTest, CreateEmpty) {
  auto mmu = MMUFactory::Create();
  EXPECT_TRUE(static_cast<detail::MMU*>(mmu.get())->memory_banks_.empty());
}

TEST_F(MMUFactoryTest, Create) {
  std::vector<std::unique_ptr<MemoryBank>> memory_banks;
  for (int i = 0; i < 3; i++) {
    auto memory_bank = std::make_unique<MockMemoryBank>();
    memory_banks.push_back(std::move(memory_bank));
  }
  auto mmu = MMUFactory::Create(std::move(memory_banks));
  EXPECT_FALSE(static_cast<detail::MMU*>(mmu.get())->memory_banks_.empty());
  EXPECT_EQ(3, static_cast<detail::MMU*>(mmu.get())->memory_banks_.size());
}

}  // namespace test
}  // namespace core
}  // namespace nesdev
