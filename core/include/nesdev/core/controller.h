/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CONTROLLER_H_
#define _NESDEV_CORE_CONTROLLER_H_
#include "nesdev/core/exceptions.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class Controller {
 public:
  explicit Controller() {};

  virtual ~Controller() = default;

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  union {
    Byte value;
    Bitfield<0, 1, Byte> up;
    Bitfield<1, 1, Byte> down;
    Bitfield<2, 1, Byte> left;
    Bitfield<3, 1, Byte> right;
    Bitfield<4, 1, Byte> start;
    Bitfield<5, 1, Byte> select;
    Bitfield<6, 1, Byte> a;
    Bitfield<7, 1, Byte> b;
  } piso = {0x00};
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CONTROLLER_H_
