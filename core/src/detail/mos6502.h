/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MOS6502_H_
#define _NESDEV_CORE_DETAIL_MOS6502_H_
#include "nesdev/core/cpu.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"
#include "macros.h"
#include "pipeline.h"

namespace nesdev {
namespace core {
namespace detail {

class MOS6502 final : public CPU {
 public:
  static const Address kBRKAddress = {0xFFFE};

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

    const Byte kCarry = 1u << 0u;
    const Byte kZeor = 1u << 1u;
  };

 public:
  MOS6502(Registers* const registers, MMU* const mmu);

  ~MOS6502();

  void Tick() override;

  void Fetch() override;

  void Reset() noexcept override;

  void IRQ() noexcept override;

  void NMI() noexcept override;

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  class Stack {
   public:
    static const Address kOffset = {0x0100};

    Stack(Registers* const registers, MMU* const mmu);

    Byte Pull() const;

    void Push(const Byte& byte);

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    Registers* const registers_;

    MMU* const mmu_;
  };

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Registers* const registers_;

  MMU* const mmu_;

  Stack stack_;

  Pipeline pipeline_;
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_CPU_H_
