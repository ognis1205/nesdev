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
#include "mocks/memory_bank.h"

namespace nesdev {
namespace core {
namespace detail {

class MMUTest : public testing::Test {
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

  detail::MMU mmu_;
};

TEST_F(MMUTest, Clear) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  mmu_.Clear();
  EXPECT_TRUE(mmu_.memory_banks_.empty());
}

TEST_F(MMUTest, Add) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_EQ(1, mmu_.memory_banks_.size());
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_EQ(2, mmu_.memory_banks_.size());
}

TEST_F(MMUTest, Set) {
  std::vector<std::unique_ptr<MemoryBank>> memory_banks;
  for (int i = 0; i < 3; i++) {
    auto memory_bank = std::make_unique<mocks::MemoryBank>();
    memory_banks.push_back(std::move(memory_bank));
  }
  mmu_.Set(std::move(memory_banks));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_EQ(3, mmu_.memory_banks_.size());
}

TEST_F(MMUTest, ReadWithValidAddress) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(true));
  EXPECT_CALL(*memory_bank, Read(testing::_))
    .Times(1)
    .WillOnce(testing::Return(0x01));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_EQ(0x01, mmu_.Read(0x0000));
}

TEST_F(MMUTest, ReadWithInvalidAddress) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(false));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_THROW(mmu_.Read(0x0000), InvalidAddress);
}

TEST_F(MMUTest, WriteWithValidAddress) {
  Byte memory = 0x00;
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(true));
  EXPECT_CALL(*memory_bank, Write(testing::_, testing::_))
    .Times(1)
    .WillOnce(testing::Assign(&memory, 0x01));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  mmu_.Write(0x0000, 0x01);
  EXPECT_EQ(0x01, memory);
}

TEST_F(MMUTest, WriteWithInvalidAddress) {
  Byte memory = 0x00;
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(false));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_THROW(mmu_.Write(0x0000, 0x01), InvalidAddress);
  EXPECT_EQ(0x00, memory);
}

TEST_F(MMUTest, SwitchWithValidAddress) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(true));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_TRUE(mmu_.Switch(0x0000));
}

TEST_F(MMUTest, SwitchWithInvalidAddress) {
  auto memory_bank = std::make_unique<mocks::MemoryBank>();
  EXPECT_CALL(*memory_bank, HasValidAddress(testing::_))
    .Times(1)
    .WillOnce(testing::Return(false));
  mmu_.Add(std::move(memory_bank));
  EXPECT_FALSE(mmu_.memory_banks_.empty());
  EXPECT_FALSE(mmu_.Switch(0x0000));
}

}  // namespace detail
}  // namespace core
}  // namespace nesdev

