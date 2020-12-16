/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "bus.h"

namespace nes {
namespace bus {

/*
 * Construct Bus instance:
 * Set/Clear RAM values and connect CPU into self.
 */
Bus::Bus() {
  cpu.ConnectTo(this);
  for (auto &i : ram) i = 0x00;
}

/*
 * Destruct MOS6502 instance.
 */
Bus::~Bus() {
  // Nothing to do.
}

/*
 * Read data at specified location.
 */
types::uint8_t Bus::Read(uint16_t address, bool read_only) {
  return ram[address];
}

/*
 * Write data at specified location.
 */
void Bus::Write(types::uint16_t address, types::uint8_t data) {
  ram[address] = data;
}

}  // namespace bus
}  // namespace nes
