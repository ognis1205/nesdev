/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_BUS_H_
#define _NES_BUS_H_
#include <array>

#include "types.h"

namespace nes {
namespace cpu {

class CPU6502;

}  // namespace cpu
}  // namespace nes

namespace nes {
namespace bus {

class Bus {
 public:
  static constexpr size_t kRAMSize = 64 * 1024;

  
  nes::cpu::CPU6502 cpu;
  std::array<types::uint8_t, kRAMSize> ram;
  types::uint8_t Read(types::uint16_t address, bool read_only = false);
  void Write(types::uint16_t address, types::uint8_t data);
};

}  // namespace bus
}  // namespace nes
#endif
