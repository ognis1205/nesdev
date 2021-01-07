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

  virtual void Next() = 0;

  virtual void RST() noexcept = 0;

  virtual void IRQ() noexcept = 0;

  virtual void NMI() noexcept = 0;

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
  };

  virtual Byte Fetch() noexcept = 0;

  Byte Fetched() const noexcept {
    return context_.fetched;
  }

  Byte Opcode() const noexcept {
    return context_.opcode_byte;
  }

  nesdev::core::Instruction Instruction() const noexcept {
    return context_.opcode->instruction;
  }

  nesdev::core::AddressingMode AddressingMode() const noexcept {
    return context_.opcode->addressing_mode;
  }

  nesdev::core::MemoryAccess MemoryAccess() const noexcept {
    return context_.opcode->memory_access;
  }

  nesdev::core::Address Address() const noexcept {
    return context_.address.effective;
  }

  Byte Offset() const noexcept {
    return context_.address.lo;
  }

  Byte Page() const noexcept {
    return context_.address.hi;
  }

  void Address(nesdev::core::Address address) noexcept {
    context_.address.effective = address;
  }

  void Address(nesdev::core::Address address, Byte offset) noexcept {
    context_.is_page_crossed = ((address + offset) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + offset;
  }

  void Address(nesdev::core::Address address, nesdev::core::Address relative) noexcept {
    context_.is_page_crossed = ((address + relative) & 0xFF00) != (address & 0xFF00);
    context_.address.effective = address + relative;
  }

  void Offset(Byte offset) noexcept {
    context_.address.lo = offset;
  }

  void Page(Byte page) noexcept {
    context_.address.hi = page;
  }

  bool CrossPage() {
    return context_.is_page_crossed;
  }

  bool If(nesdev::core::Instruction instruction) const noexcept {
    return instruction == Instruction();
  }

  bool If(nesdev::core::AddressingMode addressing_mode) const noexcept {
    return addressing_mode == AddressingMode();
  }

  bool If(nesdev::core::MemoryAccess memory_access) const noexcept {
    return memory_access == MemoryAccess();
  }

  bool IfNot(nesdev::core::Instruction instruction) const noexcept {
    return !If(instruction);
  }

  bool IfNot(nesdev::core::AddressingMode addressing_mode) const noexcept {
    return !If(addressing_mode);
  }

  bool IfNot(nesdev::core::MemoryAccess memory_access) const noexcept {
    return !If(memory_access);
  }

 protected:
  Context context_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CPU_H_
