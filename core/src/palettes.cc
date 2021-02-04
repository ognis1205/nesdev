/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <vector>
#include "nesdev/core/palettes.h"
#include "nesdev/core/types.h"

namespace {

using namespace nesdev::core;

/*
 * Predefined pallete stored in VGA Pallete format.
 * [SEE] https://wiki.nesdev.com/w/index.php/.pal
 */
const std::vector<Byte> rp2c02 = {
   84,  84,  84, // 0x00
    0,  30, 116, // 0x01
    8,  16, 144, // 0x02
   48,   0, 136, // 0x03
   68,   0, 100, // 0x04
   92,   0,  48, // 0x05
   84,   4,   0, // 0x06
   60,  24,   0, // 0x07
   32,  42,   0, // 0x08
    8,  58,   0, // 0x09
    0,  64,   0, // 0x0A
    0,  60,   0, // 0x0B
    0,  50,  60, // 0x0C
    0,   0,   0, // 0x0D
    0,   0,   0, // 0x0E
    0,   0,   0, // 0x0F

  152, 150, 152, // 0x10
    8,  76, 196, // 0x11
   48,  50, 236, // 0x12
   92,  30, 228, // 0x13
  136,  20, 176, // 0x14
  160,  20, 100, // 0x15
  152,  34,  32, // 0x16
  120,  60,   0, // 0x17
   84,  90,   0, // 0x18
   40, 114,   0, // 0x19
    8, 124,   0, // 0x1A
    0, 118,  40, // 0x1B
    0, 102, 120, // 0x1C
    0,   0,   0, // 0x1D
    0,   0,   0, // 0x1E
    0,   0,   0, // 0x1F

  236, 238, 236, // 0x20
   76, 154, 236, // 0x21
  120, 124, 236, // 0x22
  176,  98, 236, // 0x23
  228,  84, 236, // 0x24
  236,  88, 180, // 0x25
  236, 106, 100, // 0x26
  212, 136,  32, // 0x27
  160, 170,   0, // 0x28
  116, 196,   0, // 0x29
   76, 208,  32, // 0x2A
   56, 204, 108, // 0x2B
   56, 180, 204, // 0x2C
   60,  60,  60, // 0x2D
    0,   0,   0, // 0x2E
    0,   0,   0, // 0x2F

  236, 238, 236, // 0x30
  168, 204, 236, // 0x31
  188, 188, 236, // 0x32
  212, 178, 236, // 0x33
  236, 174, 236, // 0x34
  236, 174, 212, // 0x35
  236, 180, 176, // 0x36
  228, 196, 144, // 0x37
  204, 210, 120, // 0x38
  180, 222, 120, // 0x39
  168, 226, 144, // 0x3A
  152, 226, 180, // 0x3B
  160, 214, 228, // 0x3C
  160, 162, 160, // 0x3D
    0,   0,   0, // 0x3E
    0,   0,   0  // 0x3F
};

}

namespace nesdev {
namespace core {

std::vector<Byte> Palettes::RP2C02() {
  return ::rp2c02;
}

}  // namespace core
}  // namespace nesdev
