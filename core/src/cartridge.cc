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
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

//Cartridge::Cartridge(const std::string& path) {
//  std::ifstream ifs;
//  ifs.open(path, std::ifstream::binary);
//  if (ifs.is_open()) {
//    header_.Load(&ifs);
//
//    if (ContainsTrainer()) ifs.seekg(INESHeader::kTrainerSize, std::ios_base::cur);
//
//    switch(Format()) {
//    case INESFormat::NES10:
//    case INESFormat::NES20:
//    default:
//      NESDEV_CORE_THROW(InvalidHeader::Occur("Invalid iNES format detected"));
//    }
//
//    ifs.close();
//  }
//}

}  // namespace core
}  // namespace nesdev
