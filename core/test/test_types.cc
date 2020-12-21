/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <functional>

#include "types.h"

struct Fixture {
  union {
    nes::core::Address value;
    nes::core::Bitfield<0, 8, nes::core::Address> offset;
    nes::core::Bitfield<8, 8, nes::core::Address> page;
  } pc = {0x0000};

  union {
    nes::core::Byte value;
    nes::core::Bitfield<0, 1, nes::core::Byte> carry;
    nes::core::Bitfield<1, 1, nes::core::Byte> zero;
    nes::core::Bitfield<2, 1, nes::core::Byte> irq_disable;
    nes::core::Bitfield<3, 1, nes::core::Byte> decimal_mode;
    nes::core::Bitfield<4, 1, nes::core::Byte> brk_command;
    nes::core::Bitfield<5, 1, nes::core::Byte> unused;
    nes::core::Bitfield<6, 1, nes::core::Byte> overflow;
    nes::core::Bitfield<7, 1, nes::core::Byte> negative;
  } p = {0x00};
};

BOOST_FIXTURE_TEST_SUITE(Types, Fixture)

BOOST_AUTO_TEST_CASE(Bitfield) {
  BOOST_CHECK(pc.value == 0x0000);
  BOOST_CHECK(pc.offset == 0x00);
  BOOST_CHECK(pc.page == 0x00);

  pc.value = 0x0101;
  BOOST_CHECK(pc.value == 0x0101);
  BOOST_CHECK(pc.offset == 0x01);
  BOOST_CHECK(pc.page == 0x01);

  pc.page = 0x00;
  BOOST_CHECK(pc.value == 0x0001);
  BOOST_CHECK(pc.offset == 0x01);
  BOOST_CHECK(pc.page == 0x00);

  pc.offset = 0x00;
  BOOST_CHECK(pc.value == 0x0000);
  BOOST_CHECK(pc.offset == 0x00);
  BOOST_CHECK(pc.page == 0x00);

  BOOST_CHECK(p.value == 0b00000000);
  BOOST_CHECK(p.carry == 0b0);
  BOOST_CHECK(p.zero == 0b0);
  BOOST_CHECK(p.irq_disable == 0b0);
  BOOST_CHECK(p.decimal_mode == 0b0);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.carry = true;
  BOOST_CHECK(p.value == 0b00000001);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b0);
  BOOST_CHECK(p.irq_disable == 0b0);
  BOOST_CHECK(p.decimal_mode == 0b0);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.zero = true;
  BOOST_CHECK(p.value == 0b00000011);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b0);
  BOOST_CHECK(p.decimal_mode == 0b0);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.irq_disable = true;
  BOOST_CHECK(p.value == 0b00000111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b0);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.decimal_mode = true;
  BOOST_CHECK(p.value == 0b00001111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b1);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.brk_command = true;
  BOOST_CHECK(p.value == 0b00011111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b1);
  BOOST_CHECK(p.brk_command == 0b1);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.unused = true;
  BOOST_CHECK(p.value == 0b00111111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b1);
  BOOST_CHECK(p.brk_command == 0b1);
  BOOST_CHECK(p.unused == 0b1);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);

  p.overflow = true;
  BOOST_CHECK(p.value == 0b01111111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b1);
  BOOST_CHECK(p.brk_command == 0b1);
  BOOST_CHECK(p.unused == 0b1);
  BOOST_CHECK(p.overflow == 0b1);
  BOOST_CHECK(p.negative == 0b0);

  p.negative = true;
  BOOST_CHECK(p.value == 0b11111111);
  BOOST_CHECK(p.carry == 0b1);
  BOOST_CHECK(p.zero == 0b1);
  BOOST_CHECK(p.irq_disable == 0b1);
  BOOST_CHECK(p.decimal_mode == 0b1);
  BOOST_CHECK(p.brk_command == 0b1);
  BOOST_CHECK(p.unused == 0b1);
  BOOST_CHECK(p.overflow == 0b1);
  BOOST_CHECK(p.negative == 0b1);

  p.value = false;
  BOOST_CHECK(p.value == 0b00000000);
  BOOST_CHECK(p.carry == 0b0);
  BOOST_CHECK(p.zero == 0b0);
  BOOST_CHECK(p.irq_disable == 0b0);
  BOOST_CHECK(p.decimal_mode == 0b0);
  BOOST_CHECK(p.brk_command == 0b0);
  BOOST_CHECK(p.unused == 0b0);
  BOOST_CHECK(p.overflow == 0b0);
  BOOST_CHECK(p.negative == 0b0);
}

BOOST_AUTO_TEST_SUITE_END()
