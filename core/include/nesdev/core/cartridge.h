/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CARTRIDGE_H_
#define _NESDEV_CORE_CARTRIDGE_H_
#include <string>
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

/*
 * The iNES format header.
 * [SEE] https://wiki.nesdev.com/w/index.php/INES
 * [SEE] http://nesdev.com/neshdr20.txt
 * [SEE] https://nescartdb.com/
 */
class Cartridge {
 public:
  Cartridge(const std::string& path);

  ~Cartridge();

 private:
  INESHeader header_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_INES_HEADER_H_
