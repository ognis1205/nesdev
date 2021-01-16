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

namespace {

//std::size_t SizeOf(std::istream& is) {
//  is.seekg(0, std::ios::end);
//  std::istream::pos_type size = is.tellg();
//  is.seekg(0, std::ios::beg);
//  NESDEV_CORE_CASSERT(
//    size >= 0,
//    "ROM file must be non-empty");
//  NESDEV_CORE_CASSERT(
//    static_cast<uint64_t>(size) <= std::numeric_limits<std::size_t>::max(),
//    "ROM file exceeds the file size limits");
//  return static_cast<std::size_t>(size);
//}

}

namespace nesdev {
namespace core {

std::unique_ptr<ROM> ROMFactory::NROM(std::istream& is) {
//  std::vector<Byte> image(::SizeOf(is));
//  if (image.size() < sizeof(ROM::Header))
//    NESDEV_CORE_THROW(InvalidROM::Occur("Specified file does not have enough space to store heaader"));
//  if (!is.read(reinterpret_cast<char*>(image.data()), image.size()))
//    NESDEV_CORE_THROW(InvalidROM::Occur("Unable to load image from specified file"));

  std::shared_ptr<ROM::Header> header = std::make_shared<ROM::Header>();
  is.read(reinterpret_cast<char*>(header.get()), sizeof(ROM::Header));
  if (!header->HasValidMagic())
    NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible file format to iNES"));
  if (header->Mapper() != 0)
    NESDEV_CORE_THROW(InvalidROM::Occur("Incompatible mapper specified to NROM"));
  if (header->ContainsTrainer())
    is.seekg(512, std::ios_base::cur);

  std::shared_ptr<ROM::Chips> chips = std::make_shared<ROM::Chips>();
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

  return static_cast<std::unique_ptr<ROM>>(std::make_unique<detail::roms::NROM>(header.get(), chips.get(), mapper.get()));
}

}  // namespace core
}  // namespace nesdev
