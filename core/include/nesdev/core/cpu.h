/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CPU_H_
#define _NESDEV_CORE_CPU_H_
#include <cstddef>
#include <optional>
#include "nesdev/core/clock.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class CPU : public Clock {
 public:
  struct Registers {
    // Accumulator
    union {
      Byte value;
    } a = {0x00};
    // X index register
    union {
      Byte value;
    } x = {0x00};
    // Y index register
    union {
      Byte value;
    } y = {0x00};
    // Stack pointer
    union {
      Byte value;
    } s = {0x00};
    // Program counter
    union {
      Address value;
      Bitfield<0, 8, Address> lo;
      Bitfield<8, 8, Address> hi;
    } pc = {0x0000};
    // Status register
    union {
      Byte value;
      Bitfield<0, 1, Byte> carry;
      Bitfield<1, 1, Byte> zero;
      Bitfield<2, 1, Byte> irq_disable;
      Bitfield<3, 1, Byte> decimal_mode;
      Bitfield<4, 1, Byte> brk_command;
      Bitfield<5, 1, Byte> unused;
      Bitfield<6, 1, Byte> overflow;
      Bitfield<7, 1, Byte> negative;
    } p = {0x00};
  };

 public:
  virtual ~CPU() = default;

  virtual void Tick() override = 0;

  virtual void Next() = 0;

  virtual Byte Fetch() = 0;

  virtual bool IsIdle() const = 0;

  virtual void Reset() = 0;

  virtual void IRQ() = 0;

  virtual void NMI() = 0;

  virtual Address PCRegister() const = 0;

  virtual Byte ARegister() const = 0;

  virtual Byte XRegister() const = 0;

  virtual Byte YRegister() const = 0;

  virtual Byte SRegister() const = 0;

  virtual Byte PRegister() const = 0;

 public:
  [[nodiscard]]
  std::size_t Cycle() const {
    return context_.cycle;
  }

  [[nodiscard]]
  Byte Fetched() const {
    return context_.fetched;
  }

  Byte Fetched(Byte byte) {
    return context_.fetched = byte;
  }

  [[nodiscard]]
  Byte Op() const {
    return context_.opcode_byte;
  }

  [[nodiscard]]
  Instruction Inst() const {
    return context_.opcode->instruction;
  }

  [[nodiscard]]
  AddressingMode AddrMode() const {
    return context_.opcode->addressing_mode;
  }

  [[nodiscard]]
  MemoryAccess MemAccess() const {
    return context_.opcode->memory_access;
  }

  [[nodiscard]]
  Address Addr() const {
    return context_.address.effective;
  }

  [[nodiscard]]
  Byte AddrLo() const {
    return context_.address.lo;
  }

  [[nodiscard]]
  Byte AddrHi() const {
    return context_.address.hi;
  }

  [[nodiscard]]
  Address Ptr() const {
    return context_.pointer.effective;
  }

  [[nodiscard]]
  Byte PtrLo() const {
    return context_.pointer.lo;
  }

  [[nodiscard]]
  Byte PtrHi() const {
    return context_.pointer.hi;
  }

  bool CrossPage() {
    return context_.is_page_crossed;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      opcode.reset();
      cycle             = {0};
      fetched           = {0x00};
      opcode_byte       = {0x00};
      is_page_crossed   = false;
      address.effective = {0x0000};
      pointer.effective = {0x0000};
    }

    std::size_t cycle = {0};

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

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  void Addr(Address address) {
    context_.address.effective = address;
  }

  void Addr(Address address, Byte offset) {
    context_.is_page_crossed = ((address + offset) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + offset;
  }

  void Addr(Address address, Address relative) {
    context_.is_page_crossed = ((address + relative) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + relative;
  }

  void AddrLo(Byte lo) {
    context_.address.lo = lo;
  }

  void AddrHi(Byte hi) {
    context_.address.hi = hi;
  }

  void Ptr(Address address) {
    context_.pointer.effective = address;
  }

  void PtrLo(Byte lo) {
    context_.pointer.lo = lo;
  }

  void PtrHi(Byte hi) {
    context_.pointer.hi = hi;
  }

  [[nodiscard]]
  bool If(Instruction instruction) const {
    return instruction == Inst();
  }

  [[nodiscard]]
  bool If(AddressingMode addressing_mode) const {
    return addressing_mode == AddrMode();
  }

  [[nodiscard]]
  bool If(MemoryAccess memory_access) const {
    return memory_access == MemAccess();
  }

  [[nodiscard]]
  bool IfNot(Instruction instruction) const {
    return !If(instruction);
  }

  [[nodiscard]]
  bool IfNot(AddressingMode addressing_mode) const {
    return !If(addressing_mode);
  }

  [[nodiscard]]
  bool IfNot(MemoryAccess memory_access) const {
    return !If(memory_access);
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
