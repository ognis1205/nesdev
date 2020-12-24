/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <functional>
#include <boost/test/included/unit_test.hpp>
#include "nes/core/pipeline.h"

struct Fixture {
  int acc = 0;

  nes::core::Pipeline pipeline1, pipeline2;

  std::function<void()> step1 = [this]() {
    acc++;
  };

  std::function<nes::core::Pipeline::Status()> step2 = []() {
    return nes::core::Pipeline::Status::Skip;
  };
};

BOOST_FIXTURE_TEST_SUITE(Pipeline, Fixture)

BOOST_AUTO_TEST_CASE(Push) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Push(step1);
  BOOST_CHECK(!pipeline1.Done());
}

BOOST_AUTO_TEST_CASE(Stage) {
  BOOST_CHECK(pipeline1.Done());
  pipeline2.Push(step1);
  pipeline1.Stage(pipeline2);
  BOOST_CHECK(!pipeline1.Done());
}

BOOST_AUTO_TEST_CASE(Done) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Push(step1);
  BOOST_CHECK(!pipeline1.Done());
  BOOST_CHECK(acc == 0);
}

BOOST_AUTO_TEST_CASE(Clear) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Push(step1);
  BOOST_CHECK(!pipeline1.Done());
  pipeline1.Clear();
  BOOST_CHECK(pipeline1.Done());
  BOOST_CHECK(acc == 0);
}

BOOST_AUTO_TEST_CASE(Tick) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Push(step1);
  pipeline1.Push(step1);
  pipeline1.Push(step2);
  pipeline1.Push(step2);
  pipeline1.Push(step1);
  pipeline1.Tick();
  BOOST_CHECK(acc == 1);
  BOOST_CHECK(!pipeline1.Done());
  pipeline1.Tick();
  BOOST_CHECK(acc == 2);
  BOOST_CHECK(!pipeline1.Done());
  pipeline1.Tick();
  BOOST_CHECK(acc == 3);
  BOOST_CHECK(pipeline1.Done());
}

BOOST_AUTO_TEST_SUITE_END()
