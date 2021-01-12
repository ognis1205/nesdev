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
#include "nesdev/core/cartridge.h"
#include "nesdev/core/cartridge_factory.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/macros.h"
#include "detail/cartridge.h"

namespace {

std::size_t SizeOf(std::istream& is) {
  is.seekg(0, std::ios::end);
  std::istream::pos_type size = is.tellg();
  is.seekg(0, std::ios::beg);
  NESDEV_CORE_CASSERT(
    size >= 0,
    "Cartridge file must be non-empty");
  NESDEV_CORE_CASSERT(
    static_cast<uint64_t>(size) <= std::numeric_limits<std::size_t>::max(),
    "Cartridge file exceeds the file size limits");
  return static_cast<std::size_t>(size);
}

}

namespace nesdev {
namespace core {

std::unique_ptr<Cartridge> CartridgeFactory::Create(std::istream& is) {
  std::vector<Byte> image(::SizeOf(is));
  if (image.size() < sizeof(INESHeader))
    NESDEV_CORE_THROW(InvalidCartridge::Occur("Specified file does not have enough space to store heaader"));
  if (!is.read(reinterpret_cast<char*>(image.data()), image.size()))
    NESDEV_CORE_THROW(InvalidCartridge::Occur("Unable to load image from specified file"));

  INESHeader header;
  std::memcpy(&header, image.data(), sizeof(INESHeader));
  if (!header.HasValidMagic())
    NESDEV_CORE_THROW(InvalidCartridge::Occur("Incompatible file format to iNES"));
  if (image.size() != sizeof(INESHeader) + header.SizeOfPRGRom() + header.SizeOfCHRRom())
    NESDEV_CORE_THROW(InvalidCartridge::Occur("Unexpected cartridge size"));

  std::vector<Byte> prg_rom(
    image.begin() + sizeof(INESHeader),
    image.begin() + sizeof(INESHeader) + header.SizeOfPRGRom());
  std::vector<Byte> chr_rom(
    image.begin() + sizeof(INESHeader) + header.SizeOfPRGRom(),
    image.begin() + sizeof(INESHeader) + header.SizeOfPRGRom() + header.SizeOfCHRRom());

  return std::make_unique<detail::Cartridge>(header, prg_rom, chr_rom);
}

}  // namespace core
}  // namespace nesdev
