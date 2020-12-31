/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_CLOCK_H_
#define _NESDEV_CORE_CLOCK_H_

namespace nesdev {
namespace core {

class Clock {
 public:
  virtual ~Clock() = default;

  virtual void Tick() = 0;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_CLOCK_H_
