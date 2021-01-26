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

  virtual Byte Fetch() noexcept = 0;

  virtual void Reset() noexcept = 0;

  virtual void IRQ() noexcept = 0;

  virtual void NMI() noexcept = 0;

 public:
  [[nodiscard]]
  std::size_t Cycle() const noexcept {
    return context_.cycle;
  }

  [[nodiscard]]
  Byte Fetched() const noexcept {
    return context_.fetched;
  }

  [[nodiscard]]
  Byte Op() const noexcept {
    return context_.opcode_byte;
  }

  [[nodiscard]]
  Instruction Inst() const noexcept {
    return context_.opcode->instruction;
  }

  [[nodiscard]]
  AddressingMode AddrMode() const noexcept {
    return context_.opcode->addressing_mode;
  }

  [[nodiscard]]
  MemoryAccess MemAccess() const noexcept {
    return context_.opcode->memory_access;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  struct Context {
    void Clear() {
      opcode.reset();
      cycle             = {0};
      fetched           = {0x00};
      opcode_byte       = {0x00};
      is_page_crossed   = false;
      is_rst_signaled   = false;
      is_irq_signaled   = false;
      is_nmi_signaled   = false;
      address.effective = {0x0000};
      pointer.effective = {0x0000};
    }

    std::size_t cycle = {0};

    Byte fetched = {0x00};

    Byte opcode_byte = {0x00};

    std::optional<Opcode> opcode;

    bool is_page_crossed = false;

    bool is_rst_signaled = false;

    bool is_irq_signaled = false;

    bool is_nmi_signaled = false;
    
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
  [[nodiscard]]
  Address Addr() const noexcept {
    return context_.address.effective;
  }

  [[nodiscard]]
  Byte AddrLo() const noexcept {
    return context_.address.lo;
  }

  [[nodiscard]]
  Byte AddrHi() const noexcept {
    return context_.address.hi;
  }

  [[nodiscard]]
  Address Ptr() const noexcept {
    return context_.pointer.effective;
  }

  [[nodiscard]]
  Byte PtrLo() const noexcept {
    return context_.pointer.lo;
  }

  [[nodiscard]]
  Byte PtrHi() const noexcept {
    return context_.pointer.hi;
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

  void Ptr(Address address) noexcept {
    context_.pointer.effective = address;
  }

  void PtrLo(Byte lo) noexcept {
    context_.pointer.lo = lo;
  }

  void PtrHi(Byte hi) noexcept {
    context_.pointer.hi = hi;
  }

  bool CrossPage() noexcept {
    return context_.is_page_crossed;
  }

  [[nodiscard]]
  bool If(Instruction instruction) const noexcept {
    return instruction == Inst();
  }

  [[nodiscard]]
  bool If(AddressingMode addressing_mode) const noexcept {
    return addressing_mode == AddrMode();
  }

  [[nodiscard]]
  bool If(MemoryAccess memory_access) const noexcept {
    return memory_access == MemAccess();
  }

  [[nodiscard]]
  bool IfNot(Instruction instruction) const noexcept {
    return !If(instruction);
  }

  [[nodiscard]]
  bool IfNot(AddressingMode addressing_mode) const noexcept {
    return !If(addressing_mode);
  }

  [[nodiscard]]
  bool IfNot(MemoryAccess memory_access) const noexcept {
    return !If(memory_access);
  }

  [[nodiscard]]
  bool IfReset() const noexcept {
    return context_.is_rst_signaled;
  }

  [[nodiscard]]
  bool IfIRQ() const noexcept {
    return context_.is_irq_signaled;
  }

  [[nodiscard]]
  bool IfNMI() const noexcept {
    return context_.is_nmi_signaled;
  }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
