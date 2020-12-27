/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_IMPL_MOS6502_H_
#define _NES_CORE_IMPL_MOS6502_H_
#include "nes/core/cpu.h"
#include "nes/core/opcodes.h"
#include "nes/core/types.h"
#include "pipeline.h"

namespace nes {
namespace core {
namespace impl {

class MOS6502 final : public CPU {
 public:
  MOS6502();

  ~MOS6502();

  void Tick() override;

  void Reset() noexcept override;

  void IRQ() noexcept override;

  void NMI() noexcept override;

  Byte Read(const Address& address) const override;

  void Write(const Address& address, const Byte& byte) override;

 private:
  void With(
    const AddressingMode& addressing_mode,
    const MemoryAccess& memory_access,
    Pipeline* pipeline,
    void (MOS6502::*instruction)(Pipeline*));

  void Add(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void And(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void ArithmeticShiftLeft(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Branch(
    const Instruction& instruction,
    Pipeline* pipeline);

  void TestMemoryBits(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void SoftwareInterrupt(
    const Instruction& instruction,
    Pipeline* pipeline);

  void ClearStatusFlags(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Compare(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Decrement(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void ExclusiveOr(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Increment(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Jump(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Load(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void LogicalShiftRight(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  [[maybe_unused]]
  void BlockMove(
    const Instruction& instruction,
    Pipeline* pipeline);

  void NoOperation(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Or(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

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
    Pipeline* pipeline) noexcept;

  void Return(
    const Instruction& instruction,
    Pipeline* pipeline);

  void Subtract(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

  void Set(
    const Instruction& instruction,
    Pipeline* pipeline) noexcept;

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

}  // namespace impl
}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_IMPL_CPU_H_
