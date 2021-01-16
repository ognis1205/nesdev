/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstddef>
#include <cstring>
#include <iosfwd>
#include <istream>
#include <limits>
#include <memory>
#include "nesdev/core/rom.h"
#include "nesdev/core/rom_factory.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
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

  std::unique_ptr<ROM::Chips> chips = std::make_unique<ROM::Chips>();
  std::unique_ptr<ROM::Mapper> mapper = std::make_unique<detail::roms::Mapper000>(header.get(), chips.get());
  chips->prg_rom.resize(header->SizeOfPRGRom());
  chips->prg_ram.resize(header->SizeOfPRGRam());
  chips->chr_rom.resize(header->SizeOfCHRRom());

  switch (header->Format()) {
  case ROM::Header::Format::NES10:
    is.read(reinterpret_cast<char*>(chips->prg_rom.data()), chips->prg_rom.size());
    is.read(reinterpret_cast<char*>(chips->chr_rom.data()), chips->chr_rom.size());
    break;
  case ROM::Header::Format::NES20:
    break;
  }

  return std::make_unique<detail::roms::NROM>(std::move(header), std::move(chips), std::move(mapper));
}

}  // namespace core
}  // namespace nesdev
