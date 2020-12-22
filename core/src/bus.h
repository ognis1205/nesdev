/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_BUS_H_
#define _NES_CORE_BUS_H_
#include <array>
#include "types.h"

namespace nes {
namespace core {

class Bus {
 public:
  static constexpr size_t kRAMSize = 64 * 1024;

  Bus() noexcept;

  ~Bus() noexcept;

  Byte Read(Address address/*, bool read_only = false*/);

  void Write(Address address, Byte data);

  //  nes::core::MOS6502 cpu;
  std::array<Byte, kRAMSize> ram;
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_BUS_H_
