/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <functional>
#include <boost/test/included/unit_test.hpp>
#include "nes/core/opcodes.h"

struct Fixture {
};

BOOST_FIXTURE_TEST_SUITE(Opcodes, Fixture)

BOOST_AUTO_TEST_CASE(Decode) {
  for (auto i = 0x00u; i <= 0xFFu; i++) {
    BOOST_CHECK_NO_THROW({
      auto opcode = nes::core::Decode(i);
      static_cast<void>(opcode);
    });
  }
}

BOOST_AUTO_TEST_SUITE_END()
