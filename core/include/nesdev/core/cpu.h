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

  virtual bool Next() = 0;

  virtual bool RST() noexcept = 0;

  virtual bool IRQ() noexcept = 0;

  virtual bool NMI() noexcept = 0;

 protected:
  struct Context {
    size_t cycle = {0};

    Byte fetched = {0x00};

    Byte opcode_byte = {0x00};

    std::optional<Opcode> opcode;    

    bool is_page_crossed = false;
    
    union {
      Address effective;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } address = {0x0000};

    union {
      Address effective;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } pointer = {0x0000};
  };

  virtual Byte Fetch() noexcept = 0;

  Byte Fetched() const noexcept {
    return context_.fetched;
  }

  Byte Opcode() const noexcept {
    return context_.opcode_byte;
  }

  Instruction Inst() const noexcept {
    return context_.opcode->instruction;
  }

  AddressingMode AddrMode() const noexcept {
    return context_.opcode->addressing_mode;
  }

  MemoryAccess MemAccess() const noexcept {
    return context_.opcode->memory_access;
  }

  Address Addr() const noexcept {
    return context_.address.effective;
  }

  Byte AddrLo() const noexcept {
    return context_.address.lo;
  }

  Byte AddrHi() const noexcept {
    return context_.address.hi;
  }

  void Addr(Address address) noexcept {
    context_.address.effective = address;
  }

  void Addr(Address address, Byte offset) noexcept {
    context_.is_page_crossed = ((address + offset) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + offset;
  }

  void Addr(Address address, Address relative) noexcept {
    context_.is_page_crossed = ((address + relative) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + relative;
  }

  void AddrLo(Byte lo) noexcept {
    context_.address.lo = lo;
  }

  void AddrHi(Byte hi) noexcept {
    context_.address.hi = hi;
  }

  Address Ptr() const noexcept {
    return context_.pointer.effective;
  }

  Byte PtrLo() noexcept {
    return context_.pointer.lo;
  }

  Byte PtrHi() noexcept {
    return context_.pointer.hi;
  }

  void Ptr(Address address) noexcept {
    context_.pointer.effective = address;
  }

  void PtrLo(Byte lo) noexcept {
    context_.pointer.lo = lo;
  }

  void PtrHi(Byte hi) noexcept {
    context_.pointer.hi = hi;
  }

  bool CrossPage() {
    return context_.is_page_crossed;
  }

  bool If(Instruction instruction) const noexcept {
    return instruction == Inst();
  }

  bool If(AddressingMode addressing_mode) const noexcept {
    return addressing_mode == AddrMode();
  }

  bool If(MemoryAccess memory_access) const noexcept {
    return memory_access == MemAccess();
  }

  bool IfNot(Instruction instruction) const noexcept {
    return !If(instruction);
  }

  bool IfNot(AddressingMode addressing_mode) const noexcept {
    return !If(addressing_mode);
  }

  bool IfNot(MemoryAccess memory_access) const noexcept {
    return !If(memory_access);
  }

 protected:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
