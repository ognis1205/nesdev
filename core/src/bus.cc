/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "bus.h"

namespace nes {
namespace core {

Bus::Bus() noexcept {
  //  cpu.ConnectTo(this);
  for (auto &i : ram) i = 0x00;
}

Bus::~Bus() noexcept {
  // Nothing to do.
}

Byte Bus::Read(Address address/*, bool read_only*/) { return ram[address]; }

void Bus::Write(Address address, Byte data) { ram[address] = data; }

}  // namespace core
}  // namespace nes
