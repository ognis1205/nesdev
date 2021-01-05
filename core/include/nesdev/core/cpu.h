/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CPU_H_
#define _NESDEV_CORE_CPU_H_
#include <cstdlib>
#include <optional>
#include "nesdev/core/clock.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class CPU : public Clock {
 public:
  virtual ~CPU() = default;

  virtual void Tick() override = 0;

  virtual void FetchOpcode() = 0;

  virtual void RST() noexcept = 0;

  virtual void IRQ() noexcept = 0;

  virtual void NMI() noexcept = 0;

 protected:
  struct Context {
    size_t cycle = {0};

    Byte fetched = {0x00};

    Byte opcode_byte = {0x00};

    std::optional<Opcode> opcode;    

    union {
      Address effective;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } address = {0x0000};
  };

  virtual Byte Fetch(bool immediate) noexcept = 0;

  Byte Fetched() const noexcept {
    return context_.fetched;
  }

  Byte GetOpcode() const noexcept {
    return context_.opcode_byte;
  }

  Instruction GetInstruction() const noexcept {
    return context_.opcode->instruction;
  }

  AddressingMode GetAddressingMode() const noexcept {
    return context_.opcode->addressing_mode;
  }

  MemoryAccess GetMemoryAccess() const noexcept {
    return context_.opcode->memory_access;
  }

  Address GetAddress() const noexcept {
    return context_.address.effective;
  }

  void SetAddress(Address address) noexcept {
    context_.address.effective = address;
  }

  void SetAddressLo(Byte lo) noexcept {
    context_.address.lo = lo;
  }

  void SetAddressHi(Byte hi) noexcept {
    context_.address.hi = hi;
  }

 protected:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
