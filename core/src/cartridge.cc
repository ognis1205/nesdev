/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <fstream>
#include <string>
#include "nesdev/core/cartridge.h"
#include "nesdev/core/exceptions.h"
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"
#include "macros.h"

namespace nesdev {
namespace core {

Cartridge::Cartridge(const std::string& path) : header_{} {
  std::ifstream ifs;
  ifs.open(path, std::ifstream::binary);
  if (ifs.is_open()) {
    header_.Load(&ifs);
    //    if (ContainsTrainer()) ifs.seekg(INESHeader::kTrainerSize, std::ios_base::cur);
  }
}

}  // namespace core
}  // namespace nesdev
