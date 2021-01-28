/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <cstdlib>
#include <ctime>
#include <nesdev/core.h>

namespace nc = nesdev::core;

class Utility {
 public:
  static void Init() noexcept {
    srand(time(nullptr));
  }

  template<nc::RGBA From, nc::RGBA To>
  static nc::RGBA Noise() noexcept {
    return From + rand() % (To - From + 1);
  }

 private:
  Utility();
};

#endif  // ifndef _UTILITY_H_
