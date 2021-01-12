/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CARTRIDGE_H_
#define _NESDEV_CORE_CARTRIDGE_H_
#include "nesdev/core/ines_header.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class Cartridge {
 public:
  explicit Cartridge(const INESHeader& header) : header_{header} {};

  virtual ~Cartridge() = default;

 protected:
  INESHeader header_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_INES_HEADER_H_
