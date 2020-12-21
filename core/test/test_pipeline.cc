/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include "pipeline.h"

BOOST_AUTO_TEST_CASE(test1) {
    const int x = 1;
    BOOST_CHECK(x == 1);
}
