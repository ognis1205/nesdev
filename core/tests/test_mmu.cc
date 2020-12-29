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

class MockMemoryBank : public nesdev::core::MemoryBank {
 public:
  MOCK_CONST_METHOD1(MockHasValidAddress, bool(const nesdev::core::Address&));
  MOCK_CONST_METHOD1(Read, nesdev::core::Byte(const nesdev::core::Address&));
  MOCK_METHOD2(Write, void(const nesdev::core::Address&, const nesdev::core::Byte&));
  virtual bool HasValidAddress(const nesdev::core::Address& address) const noexcept {
    return MockHasValidAddress(address);
  }
};

class MMUTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  nesdev::core::detail::MMU mmu;
};

TEST_F(MMUTest, Clear) {
  auto memory_bank = std::make_unique<MockMemoryBank>();
  mmu.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu.memory_banks_.empty());
  mmu.Clear();
  EXPECT_TRUE(mmu.memory_banks_.empty());
}

TEST_F(MMUTest, Add) {
  auto memory_bank = std::make_unique<MockMemoryBank>();
  mmu.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu.memory_banks_.empty());
  EXPECT_EQ(1, mmu.memory_banks_.size());
  mmu.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu.memory_banks_.empty());
  EXPECT_EQ(2, mmu.memory_banks_.size());
}

TEST_F(MMUTest, Set) {
  std::vector<std::unique_ptr<nesdev::core::MemoryBank>> memory_banks;
  for (int i = 0; i < 3; i++) {
    auto memory_bank = std::make_unique<MockMemoryBank>();
    memory_banks.push_back(std::move(memory_bank));
  }
  mmu.Set(std::move(memory_banks));
  EXPECT_FALSE(mmu.memory_banks_.empty());
  EXPECT_EQ(3, mmu.memory_banks_.size());
}

TEST_F(MMUTest, Read) {
  auto memory_bank = std::make_unique<MockMemoryBank>();
  EXPECT_CALL(*memory_bank, MockHasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(true));
//    .WillOnce(testing::Return(false));
  EXPECT_CALL(*memory_bank, Read(testing::_))
    .Times(1)
    .WillOnce(testing::Return(0x01));
//    .WillOnce(testing::Return(0x02));
  mmu.Add(std::move(memory_bank));
  EXPECT_EQ(0x01, mmu.Read(0u));
//  EXPECT_EQ(0x01, mmu.Read(0u));
  EXPECT_FALSE(mmu.memory_banks_.empty());
}
