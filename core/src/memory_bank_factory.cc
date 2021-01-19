/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstddef>
#include <functional>
#include <memory>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "nesdev/core/memory_bank_factory.h"
#include "nesdev/core/ppu.h"
#include "nesdev/core/rom.h"
#include "nesdev/core/types.h"
#include "detail/memory_banks/chip.h"
#include "detail/memory_banks/forward.h"

namespace {

using namespace nesdev::core;

class CPUAdapter : public MemoryBank {
 public:
  explicit CPUAdapter(ROM* const rom)
    : rom_(rom) {}

  bool HasValidAddress(Address address) const noexcept override {
    return rom_->mapper->HasValidAddress(ROM::Mapper::Space::CPU, address);
  }

  Byte Read(Address address) const override {
    return rom_->mapper->Read(ROM::Mapper::Space::CPU, address);
  }

  void Write(Address address, Byte byte) override {
    rom_->mapper->Write(ROM::Mapper::Space::CPU, address, byte);
  }

  std::size_t Size() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

  Byte* Data() override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

  const Byte* Data() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  ROM* const rom_;
};

class PPUAdapter : public MemoryBank {
 public:
  explicit PPUAdapter(ROM* const rom)
    : rom_(rom) {}

  bool HasValidAddress(Address address) const noexcept override {
    return rom_->mapper->HasValidAddress(ROM::Mapper::Space::PPU, address);
  }

  Byte Read(Address address) const override {
    return rom_->mapper->Read(ROM::Mapper::Space::PPU, address);
  }

  void Write(Address address, Byte byte) override {
    rom_->mapper->Write(ROM::Mapper::Space::PPU, address, byte);
  }

  std::size_t Size() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

  Byte* Data() override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

  const Byte* Data() const override {
    NESDEV_CORE_THROW(NotImplemented::Occur("Not implemented method operated to cpu adapter"));
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  ROM* const rom_;
};

std::function<Byte(Address)> PPUReader(PPU* ppu) {
  return [ppu](Address address) { return ppu->Read(address); };
}

std::function<void(Address, Byte)> PPUWriter(PPU* ppu) {
  return [ppu](Address address, Byte byte) { ppu->Write(address, byte); };
}

}

namespace nesdev {
namespace core {

MemoryBanks MemoryBankFactory::CPUBus(ROM *rom, PPU* ppu) {
  MemoryBanks banks;
  banks.push_back(std::make_unique<::CPUAdapter>(rom));                                                                  // ROM
  banks.push_back(std::make_unique<detail::memory_banks::Chip<0x0000, 0x1FFF>>(0x800));                                  // RAM
  banks.push_back(std::make_unique<detail::memory_banks::Forward<0x2000, 0x3FFF>>(0x8, PPUReader(ppu), PPUWriter(ppu))); // PPU
  banks.push_back(std::make_unique<detail::memory_banks::Chip<0x4000, 0x4017>>(0x18));                                   // IO
  banks.push_back(std::make_unique<detail::memory_banks::Chip<0x4018, 0x401F>>(0x8));                                    // IO device
  return banks;
}

MemoryBanks MemoryBankFactory::PPUBus(ROM *rom) {
  MemoryBanks banks;
  banks.push_back(std::make_unique<::PPUAdapter>(rom));                                // ROM
  banks.push_back(std::make_unique<detail::memory_banks::Chip<0x3F00, 0x3FFF>>(0x20)); // Pallete
  return banks;
}

}  // namespace core
}  // namespace nesdev
