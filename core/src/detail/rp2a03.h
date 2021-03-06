/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_RP2A03_H_
#define _NESDEV_CORE_DETAIL_RP2A03_H_
#include <iostream>
#include <cstdint>
#include <functional>
#include "nesdev/core/cpu.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/mmu.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"
#include "pipeline.h"

namespace nesdev {
namespace core {
namespace detail {

class RP2A03 final : public CPU {
 public:
  static const Address kBRKAddress = {0xFFFE};

  static const Address kRSTAddress = {0xFFFC};

  static const Address kNMIAddress = {0xFFFA};

 public:
  RP2A03(CPU::Registers* const registers, MMU* const mmu);

  ~RP2A03();

  void Tick() override;

  void Next() override;

  bool IsIdle() const override;

  void Reset() override;

  void IRQ() override;

  void NMI() override;

  Address PCRegister() const override {
    return registers_->pc.value;
  }

  Byte ARegister() const override {
    return registers_->a.value;
  }

  Byte XRegister() const override {
    return registers_->x.value;
  }

  Byte YRegister() const override {
    return registers_->y.value;
  }

  Byte SRegister() const override {
    return registers_->s.value;
  }

  Byte PRegister() const override {
    return registers_->p.value;
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  class Stack {
   public:
    static const Address kOffset = {0x0100};

    static const Byte kHead      = {0xFD};

    Stack(CPU::Registers* const registers, MMU* const mmu)
      : registers_{registers},
        mmu_{mmu} {}

    [[nodiscard]]
    Byte Pull() const {
      return mmu_->Read(kOffset + ++registers_->s.value);
    }

    void Push(Byte byte) {
      mmu_->Write(kOffset + registers_->s.value--, byte);
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    CPU::Registers* const registers_;

    MMU* const mmu_;
  };

  class ALU {
   public:
    union Bus {
      Word concat;
      Bitfield<0, 8, Word> b;
      Bitfield<8, 8, Word> a;
    };

    static Bus Load(Byte a, Byte b) {
      return {static_cast<Word>(a << 8 | b)};      
    };

    ALU(CPU::Registers* const registers)
      : registers_ {registers} {}

    [[nodiscard]]
    Byte ShiftL(Bus bus, bool rotate_carry) {
      bus.concat <<= 1;
      bus.b |= rotate_carry ? registers_->p.carry : 0x00;
      registers_->p.carry    = bus.a  & 0x01;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte ShiftR(Bus bus, bool rotate_carry) {
      bus.concat >>= 1;
      bus.a |= rotate_carry ? registers_->p.carry << 7 : 0x00;
      registers_->p.carry    = bus.b  & 0x80;
      registers_->p.zero     = bus.a == 0x00;
      registers_->p.negative = bus.a  & 0x80;
      return bus.a;
    }

    [[nodiscard]]
    Byte Increment(Bus bus) {
      ++bus.b;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte Decrement(Bus bus) {
      --bus.b;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte PassThrough(Bus bus) {
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte Or(Bus bus) {
      bus.b |= bus.a;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte And(Bus bus) {
      bus.b &= bus.a;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte Xor(Bus bus) {
      bus.b ^= bus.a;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    [[nodiscard]]
    Byte Add(Bus bus) {
      bus.concat = CheckOverflow(bus.a, bus.b, bus.a + bus.b + registers_->p.carry);
      registers_->p.carry    = bus.a  & 0x01;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
      return bus.b;
    }

    void Cmp(Bus bus) {
      registers_->p.carry    = bus.a >= bus.b;
      bus.concat = bus.a - bus.b;
      registers_->p.zero     = bus.b == 0x00;
      registers_->p.negative = bus.b  & 0x80;
    }

    void Bit(Bus bus) {
      bus.concat = (bus.a & bus.b) << 8 | bus.b;
      registers_->p.zero     = bus.a == 0x00;
      registers_->p.negative = bus.b  & 1u << 7;
      registers_->p.overflow = bus.b  & 1u << 6;
    }

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    // [SEE] http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
    [[nodiscard]]
    Word CheckOverflow(Word a, Word b, Word r) {
      registers_->p.overflow = (a ^ r) & (b ^ r) & 0x80;
      return r;
    };

   NESDEV_CORE_PRIVATE_UNLESS_TESTED:
    CPU::Registers* const registers_;
  };

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  /*
   * The high byte of the effective address may be invalid in indexed addressing
   * modes. Because the processor cannot undo a write to an invalid address, it
   * always reads from the address first.
   */
  static constexpr Address FixHiByte(Address address) {
    return address - static_cast<Address>(0x0100);
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Byte Fetch() override {
    if (AddrMode() == AddressingMode::IMM)
      Addr(registers_->pc.value++);
    return context_.fetched = Read(Addr());
  }

  void Stage() {
    // This is just for formatting the code.
  }

  void Stage(const std::function<void()>& step, bool when=true) {
    if (when) pipeline_.Push(step);
  }

  [[nodiscard]]
  bool ClearWhenCompleted() {
    if (pipeline_.Done()) {
      pipeline_.Clear();
      return true;
    }
    return false;
  }

  void Execute() {
    pipeline_.Tick();
  }

  void Parse() {
    context_.opcode_byte = Read(registers_->pc.value++);
    context_.opcode = Opcodes::Decode(context_.opcode_byte);
  }

  Byte Read(Address address) const {
    return mmu_->Read(address);
  }

  void Write(Address address, Byte byte) {
    mmu_->Write(address, byte);
  }

  [[nodiscard]]
  Byte Pull() const {
    return stack_.Pull();
  }

  void Push(Byte byte) {
    stack_.Push(byte);
  }

  [[nodiscard]]
  Byte ShiftL(Byte b, bool rotate_carry) {
    return alu_.ShiftL(ALU::Load(0x00, b), rotate_carry);
  }

  [[nodiscard]]
  Byte ShiftR(Byte a, bool rotate_carry) {
    return alu_.ShiftR(ALU::Load(a, 0x00), rotate_carry);
  }

  [[nodiscard]]
  Byte Increment(Byte b) {
    return alu_.Increment(ALU::Load(0x00, b));
  }

  [[nodiscard]]
  Byte Decrement(Byte b) {
    return alu_.Decrement(ALU::Load(0x00, b));
  }

  [[nodiscard]]
  Byte PassThrough(Byte b) {
    return alu_.PassThrough(ALU::Load(0x00, b));
  }

  [[nodiscard]]
  Byte Or(Byte a, Byte b) {
    return alu_.Or(ALU::Load(a, b));
  }

  [[nodiscard]]
  Byte And(Byte a, Byte b) {
    return alu_.And(ALU::Load(a, b));
  }

  [[nodiscard]]
  Byte Xor(Byte a, Byte b) {
    return alu_.Xor(ALU::Load(a, b));
  }

  [[nodiscard]]
  Byte Add(Byte a, Byte b) {
    return alu_.Add(ALU::Load(a, b));
  }

  [[nodiscard]]
  Byte Sub(Byte a, Byte b) {
    return alu_.Add(ALU::Load(a, ~b));
  }

  void Cmp(Byte a, Byte b) {
    return alu_.Cmp(ALU::Load(a, b));
  }

  void Bit(Byte a, Byte b) {
    return alu_.Bit(ALU::Load(a, b));
  }

  void FixPage() {
    // For relative addressing mode, if the specified offset is negative. Fix its value
    // to signed value in 16-bit length binary.
    if (context_.address.lo & 0x80)
      AddrHi(0xFF);
  }

  [[nodiscard]]
  bool IfCarry() const {
    return !!registers_->p.carry;
  }

  [[nodiscard]]
  bool IfZero() const {
    return !!registers_->p.zero;
  }

  [[nodiscard]]
  bool IfIRQDisable() const {
    return !!registers_->p.irq_disable;
  }

  [[nodiscard]]
  bool IfDecimalMode() const {
    return !!registers_->p.decimal_mode;
  }

  [[nodiscard]]
  bool IfBRKCommand() const {
    return !!registers_->p.brk_command;
  }

  [[nodiscard]]
  bool IfUnused() const {
    return !!registers_->p.unused;
  }

  [[nodiscard]]
  bool IfOverflow() const {
    return !!registers_->p.overflow;
  }

  [[nodiscard]]
  bool IfNegative() const {
    return !!registers_->p.negative;
  }

  [[nodiscard]]
  bool IfNotCarry() const {
    return !registers_->p.carry;
  }

  [[nodiscard]]
  bool IfNotZero() const {
    return !registers_->p.zero;
  }

  [[nodiscard]]
  bool IfNotIRQDisable() const {
    return !registers_->p.irq_disable;
  }

  [[nodiscard]]
  bool IfNotDecimalMode() const {
    return !registers_->p.decimal_mode;
  }

  [[nodiscard]]
  bool IfNotBRKCommand() const {
    return !registers_->p.brk_command;
  }

  [[nodiscard]]
  bool IfNotUnused() const {
    return !registers_->p.unused;
  }

  [[nodiscard]]
  bool IfNotOverflow() const {
    return !registers_->p.overflow;
  }

  [[nodiscard]]
  bool IfNotNegative() const {
    return !registers_->p.negative;
  }

  void Branch(Address relative) {
    context_.is_page_crossed = ((registers_->pc.value + relative) & 0xFF00) != (registers_->pc.value & 0xFF00);
    registers_->pc.value = registers_->pc.value + relative;
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  CPU::Registers* const registers_;

  MMU* const mmu_;

  Stack stack_;

  ALU alu_;

  Pipeline pipeline_;
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_RP2A03_H_
