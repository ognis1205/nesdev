/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <functional>

#include "pipeline.h"

struct Fixture {
  Fixture() : pipeline1{}, pipeline2{}, acc{0} {}

  ~Fixture() {}

  nes::core::Pipeline pipeline1, pipeline2;

  int acc;

  std::function<void()> step1 = [this]() {
    acc++;
  };

  std::function<nes::core::Pipeline::Status()> step2 = []() {
    return nes::core::Pipeline::Status::Skip;
  };
};

BOOST_FIXTURE_TEST_SUITE(Pipeline, Fixture)

BOOST_AUTO_TEST_CASE(Stage) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Stage(step1);
  BOOST_CHECK(!pipeline1.Done());
}

BOOST_AUTO_TEST_CASE(AppendTo) {
  BOOST_CHECK(pipeline2.Done());
  pipeline1.Stage(step1);
  pipeline1.AppendTo(pipeline2);
  BOOST_CHECK(!pipeline2.Done());
}

BOOST_AUTO_TEST_CASE(Done) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Stage(step1);
  BOOST_CHECK(!pipeline1.Done());
  BOOST_CHECK(acc == 0);
}

BOOST_AUTO_TEST_CASE(Clear) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Stage(step1);
  BOOST_CHECK(!pipeline1.Done());
  pipeline1.Clear();
  BOOST_CHECK(pipeline1.Done());
  BOOST_CHECK(acc == 0);
}

BOOST_AUTO_TEST_CASE(Tick) {
  BOOST_CHECK(pipeline1.Done());
  pipeline1.Stage(step1);
  pipeline1.Stage(step1);
  pipeline1.Stage(step2);
  pipeline1.Stage(step2);
  pipeline1.Stage(step1);
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
