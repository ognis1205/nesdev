/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstddef>
#include <fstream>
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

void INESHeader::Load(std::ifstream* const ifs) {
  if (ifs->is_open()) ifs->read(reinterpret_cast<char*>(this), sizeof(*this));
//  if (!HasValidMagic()) NESDEV_CORE_THROW(InvalidHeader::Occur("Incompatible file format to iNES"));
}

}  // namespace core
}  // namespace nesdev
