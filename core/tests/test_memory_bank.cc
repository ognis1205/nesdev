/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "nes/core/exceptions.h"
#include "nes/core/types.h"
#include "detail/memory_bank.h"

struct Fixture {
  nes::core::detail::MemoryBank<0x0000, 0x1FFF, 0x800> bank;
};

BOOST_FIXTURE_TEST_SUITE(MemoryBank, Fixture)

BOOST_AUTO_TEST_CASE(HasValidAddress) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    BOOST_CHECK(bank.HasValidAddress(i));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    BOOST_CHECK(!bank.HasValidAddress(i));
  }
}

BOOST_AUTO_TEST_CASE(Read) {
  auto check = [](const nes::core::InvalidAddress& e) {
    return e.what() == std::string("[nes::core::InvalidAddress] Invalid Address specified to Read.");
  };
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    BOOST_CHECK(bank.Read(i) == 0x00);
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    BOOST_CHECK_EXCEPTION(bank.Read(i), nes::core::InvalidAddress, check);
  }
}

BOOST_AUTO_TEST_CASE(Write) {
  auto check = [](const nes::core::InvalidAddress& e) {
    return e.what() == std::string("[nes::core::InvalidAddress] Invalid Address specified to Write.");
  };
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    bank.Write(i, static_cast<nes::core::Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    BOOST_CHECK(bank.Read(i) == static_cast<nes::core::Byte>(i >> 2));
  }
  for (auto i = 0x2000u; i <= 0xFFFFu; i++) {
    BOOST_CHECK_EXCEPTION(bank.Write(i, i >> 2), nes::core::InvalidAddress, check);
  }
}

BOOST_AUTO_TEST_CASE(PointerTo) {
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    bank.Write(i, static_cast<nes::core::Byte>(i >> 2));
  }
  for (auto i = 0x0000u; i <= 0x1FFFu; i++) {
    BOOST_CHECK(*(bank.PointerTo(i)) == static_cast<nes::core::Byte>(i >> 2));
    BOOST_CHECK(*(bank.PointerTo(i)) == *(bank.PointerTo(i % 0x800u)));
  }
}

//BOOST_AUTO_TEST_CASE(ToString) {
//}

BOOST_AUTO_TEST_SUITE_END()
