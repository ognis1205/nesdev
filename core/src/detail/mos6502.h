/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_MOS6502_H_
#define _NESDEV_CORE_DETAIL_MOS6502_H_
#include "nesdev/core/cpu.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/opcodes.h"
#include "nesdev/core/types.h"
#include "pipeline.h"

namespace nesdev {
namespace core {
namespace detail {

class MOS6502 final : public CPU {
 public:
  MOS6502();

  ~MOS6502();

  void Tick() override;

  void Reset() NESDEV_CORE_NOEXCEPT override;

  void IRQ() NESDEV_CORE_NOEXCEPT override;

  void NMI() NESDEV_CORE_NOEXCEPT override;

  Byte Read(const Address& address) NESDEV_CORE_CONST override;

  void Write(const Address& address, const Byte& byte) override;

 NESDEV_CORE_PRIVATE:
  void With(
    const AddressingMode& addressing_mode,
    const MemoryAccess& memory_access,
    Pipeline* pipeline,
    void (MOS6502::*instruction)(Pipeline*));

  void Add(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void And(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void ArithmeticShiftLeft(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Branch(
    const Instruction& instruction,
    Pipeline* pipeline);

  void TestMemoryBits(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void SoftwareInterrupt(
    const Instruction& instruction,
    Pipeline* pipeline);

  void ClearStatusFlags(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Compare(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Decrement(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void ExclusiveOr(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Increment(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Jump(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Load(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void LogicalShiftRight(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  [[maybe_unused]]
  void BlockMove(
    const Instruction& instruction,
    Pipeline* pipeline);

  void NoOperation(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Or(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Push(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Pull(
    const Instruction& instruction,
    Pipeline* pipeline);

  [[maybe_unused]]
  void ResetStatusBits(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Rotate(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Return(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Subtract(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  void Set(
    const Instruction& instruction,
    Pipeline* pipeline) NESDEV_CORE_NOEXCEPT;

  [[maybe_unused]]
  void Stop(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Store(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Transfer(
    const Instruction& instruction,
    Pipeline* pipeline);

  [[maybe_unused]]
  void Test(
    const Instruction& instruction,
    Pipeline* pipeline);

  [[maybe_unused]]
  void Wait(
    const Instruction& instruction,
    Pipeline* pipeline);

  [[maybe_unused]]
  void Reserved(
    const Instruction& instruction,
    Pipeline* pipeline);

  [[maybe_unused]]
  void Exchange(
    const Instruction& instruction,
    Pipeline* pipeline);
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_CPU_H_
