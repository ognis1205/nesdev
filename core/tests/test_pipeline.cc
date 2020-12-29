/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <functional>
#include <time.h>
#include <gtest/gtest.h>
#include "detail/pipeline.h"

class PipelineTest : public testing::Test {
 protected:
  void SetUp() override {
    start_time_ = time(nullptr);
  }

  void TearDown() override {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long";
  }

  time_t start_time_;

  int acc_ = 0;

  nes::core::detail::Pipeline pipeline1_, pipeline2_;

  std::function<void()> step1_ = [this]() {
    acc_++;
  };

  std::function<nes::core::detail::Pipeline::Status()> step2_ = []() {
    return nes::core::detail::Pipeline::Status::Skip;
  };
};

TEST_F(PipelineTest, Push) {
  EXPECT_TRUE(pipeline1_.Done());
  pipeline1_.Push(step1_);
  EXPECT_FALSE(pipeline1_.Done());
}

TEST_F(PipelineTest, Stage) {
  EXPECT_TRUE(pipeline1_.Done());
  pipeline2_.Push(step1_);
  pipeline1_.Stage(pipeline2_);
  EXPECT_FALSE(pipeline1_.Done());
}

TEST_F(PipelineTest, Done) {
  EXPECT_TRUE(pipeline1_.Done());
  pipeline1_.Push(step1_);
  EXPECT_FALSE(pipeline1_.Done());
  EXPECT_EQ(0, acc_);
}

TEST_F(PipelineTest, Clear) {
  EXPECT_TRUE(pipeline1_.Done());
  pipeline1_.Push(step1_);
  EXPECT_FALSE(pipeline1_.Done());
  pipeline1_.Clear();
  EXPECT_TRUE(pipeline1_.Done());
  EXPECT_EQ(0, acc_);
}

TEST_F(PipelineTest, Tick) {
  EXPECT_TRUE(pipeline1_.Done());
  pipeline1_.Push(step1_);
  pipeline1_.Push(step1_);
  pipeline1_.Push(step2_);
  pipeline1_.Push(step2_);
  pipeline1_.Push(step1_);
  pipeline1_.Tick();
  EXPECT_EQ(1, acc_);
  EXPECT_FALSE(pipeline1_.Done());
  pipeline1_.Tick();
  EXPECT_EQ(2, acc_);
  EXPECT_FALSE(pipeline1_.Done());
  pipeline1_.Tick();
  EXPECT_EQ(3, acc_);
  EXPECT_TRUE(pipeline1_.Done());
}
