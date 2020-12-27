/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_CPU_H_
#define _NES_CORE_CPU_H_
#include "nes/core/types.h"

namespace nes {
namespace core {

class CPU {
 public:
  virtual ~CPU() = default;

  virtual void Tick() = 0;

  virtual void Reset() noexcept = 0;

  virtual void IRQ() noexcept = 0;

  virtual void NMI() noexcept = 0;

  virtual Byte Read(const Address& address) const = 0;

  virtual void Write(const Address& address, const Byte& byte) = 0;
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_CPU_H_
