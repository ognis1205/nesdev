/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_TEST_MOCK_CPU_H_
#define _NESDEV_CORE_TEST_MOCK_CPU_H_
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nesdev/core.h>

namespace nesdev {
namespace core {
namespace test {

class MockCPU : public CPU {
 public:
  MOCK_METHOD0(Tick, void());

  MOCK_METHOD0(Next, bool());

  MOCK_METHOD0(MockFetch, Byte());

  MOCK_METHOD0(MockRST, bool());

  MOCK_METHOD0(MockIRQ, bool());

  MOCK_METHOD0(MockNMI, bool());

  Byte Fetch() noexcept {
    return MockFetch();
  }

  bool RST() noexcept {
    return MockRST();
  }

  bool IRQ() noexcept {
    return MockIRQ();
  }

  bool NMI() noexcept {
    return MockNMI();
  }

  Byte Fetched() const noexcept {
    return CPU::Fetched();
  }

  Byte Opcode() const noexcept {
    return CPU::Opcode();
  }

  Instruction Inst() const noexcept {
    return CPU::Inst();
  }

  AddressingMode AddrMode() const noexcept {
    return CPU::AddrMode();
  }

  MemoryAccess MemAccess() const noexcept {
    return CPU::MemAccess();
  }

  Address Addr() const noexcept {
    return CPU::Addr();
  }

  Byte AddrLo() const noexcept {
    return CPU::AddrLo();
  }

  Byte AddrHi() const noexcept {
    return CPU::AddrHi();
  }

  void Addr(Address address) noexcept {
    return CPU::Addr(address);
  }

  void Addr(Address address, Byte offset) noexcept {
    return CPU::Addr(address, offset);
  }

  void Addr(Address address, Address relative) noexcept {
    return CPU::Addr(address, relative);
  }

  void AddrLo(Byte lo) noexcept {
    return CPU::AddrLo(lo);
  }

  void AddrHi(Byte hi) noexcept {
    return CPU::AddrHi(hi);
  }

  Address Ptr() const noexcept {
    return CPU::Ptr();
  }

  Byte PtrLo() noexcept {
    return CPU::PtrLo();
  }

  Byte PtrHi() noexcept {
    return CPU::PtrHi();
  }

  void Ptr(Address address) noexcept {
    return CPU::Ptr(address);
  }

  void PtrLo(Byte lo) noexcept {
    return CPU::PtrLo(lo);
  }

  void PtrHi(Byte hi) noexcept {
    return CPU::PtrHi(hi);
  }

  bool CrossPage() noexcept {
    return CPU::CrossPage();
  }

  bool If(Instruction instruction) const noexcept {
    return CPU::If(instruction);
  }

  bool If(AddressingMode addressing_mode) const noexcept {
    return CPU::If(addressing_mode);
  }

  bool If(MemoryAccess memory_access) const noexcept {
    return CPU::If(memory_access);
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

  bool IfRST() const noexcept {
    return CPU::IfRST();
  }

  bool IfIRQ() const noexcept {
    return CPU::IfIRQ();
  }

  bool IfNMI() const noexcept {
    return CPU::IfNMI();
  }
};

}  // namespace test
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_TEST_MOCK_CPU_H_
