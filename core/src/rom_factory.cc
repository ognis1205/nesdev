/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
//#include <cstddef>
//#include <cstring>
#include <iosfwd>
#include <istream>
#include <memory>
#include "nesdev/core/rom.h"
#include "nesdev/core/rom_factory.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/memory_bank.h"
#include "detail/memory_bank.h"
#include "detail/roms/nrom.h"
#include "detail/roms/mapper000.h"

namespace nesdev {
namespace core {

std::unique_ptr<ROM> ROMFactory::NROM(std::istream& is) {
  std::unique_ptr<ROM::Header> header = std::make_unique<ROM::Header>();

  is.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));

  if (!header->HasValidMagic())
    NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible file format to iNES"));

  if (header->Mapper() != 0)
    NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible mapper specified to NROM"));

  if (header->ContainsTrainer())
    is.seekg(512, std::ios_base::cur);

  std::unique_ptr<ROM::Chips> chips;
  if (header->NumOfCHRRoms() != 0)
    chips = std::make_unique<ROM::Chips>(
      std::make_unique<detail::MemoryBank<0x0000, 0x1FFF>>(header->SizeOfCHRRom()),
      std::make_unique<detail::VoidMemory>(),
      std::make_unique<detail::MemoryBank<0x8000, 0xFFFF>>(header->SizeOfPRGRom()),
      std::make_unique<detail::MemoryBank<0x6000, 0x7FFF>>(header->SizeOfPRGRam()));
  else
    chips = std::make_unique<ROM::Chips>(
      std::make_unique<detail::VoidMemory>(),
      std::make_unique<detail::MemoryBank<0x0000, 0x1FFF>>(header->SizeOfCHRRam()),
      std::make_unique<detail::MemoryBank<0x8000, 0xFFFF>>(header->SizeOfPRGRom()),
      std::make_unique<detail::MemoryBank<0x6000, 0x7FFF>>(header->SizeOfPRGRam()));

  std::unique_ptr<ROM::Mapper> mapper = std::make_unique<detail::roms::Mapper000>(header.get(), chips.get());

  is.read(reinterpret_cast<char*>(chips->prg_rom->Data()), chips->prg_rom->Size());

  if (header->NumOfCHRRoms() != 0)
    is.read(reinterpret_cast<char*>(chips->chr_rom->Data()), chips->chr_rom->Size());
  else
    is.read(reinterpret_cast<char*>(chips->chr_ram->Data()), chips->chr_ram->Size());

  return std::make_unique<detail::roms::NROM>(std::move(header), std::move(chips), std::move(mapper));
}

}  // namespace core
}  // namespace nesdev
