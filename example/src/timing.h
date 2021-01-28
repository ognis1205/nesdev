/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _TIMING_H_
#define _TIMING_H_
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <functional>
#include <thread>
#include <nesdev/core.h>

namespace nc = nesdev::core;

class Timing {
 public:
  Timing(const nc::ROM::Header& header) {
    if (header.TVSystem() == nc::ROM::Header::TVSystem::PAL) {
      double master_clock_rate = 26601712.0;
      cpu_clock_rate_          = master_clock_rate / 16.0;                // ~1.66 MHz
      ppu_clock_rate_          = master_clock_rate / 5.0;                 // ~5.32 MHz
      fps_                     = ppu_clock_rate_   / (341 * 312);         // ~50.0 FPS
    } else {
      double master_clock_rate = 21477272.0;
      cpu_clock_rate_          = master_clock_rate / 12.0;                // ~1.79 MHz
      ppu_clock_rate_          = master_clock_rate / 4.0;                 // ~5.37 MHz
      fps_                     = ppu_clock_rate_   / (341 * 261 + 340.5); // ~60.1 FPS
    }
  }

  ~Timing() = default;

 public:
  void SleepPPU() {
    std::this_thread::sleep_for(std::chrono::milliseconds(unsigned(1e6 / ppu_clock_rate_)));
  }

  void SleepCPU() {
    std::this_thread::sleep_for(std::chrono::milliseconds(unsigned(1e6 / cpu_clock_rate_)));
  }

  double FPS() const noexcept {
    return fps_;
  }

 public:
  double cpu_clock_rate_;

  double ppu_clock_rate_;

  double fps_;
};

#endif  // ifndef _TIMING_H_
