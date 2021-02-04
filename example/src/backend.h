/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _BACKEND_H_
#define _BACKEND_H_
#include <cstddef>
#include <chrono>
#include <unistd.h>
#include <nesdev/core.h>
#include <SDL.h>

namespace nc = nesdev::core;

struct Clock {
  typedef std::chrono::duration<std::uint32_t, std::milli> duration;
  typedef std::chrono::time_point<Clock> TimePoint;
  static TimePoint Now() noexcept {
    using namespace std::chrono;
    return TimePoint(
      duration(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()));
  }
};

class Backend {
 public:
  static const int kFPS = 30;

  static constexpr int kDelay = 1000000.0f / kFPS;

 public:
  Backend(const nc::NES& nes,
	  nc::NES::Controller* const player_one,
	  nc::NES::Controller* const player_two);

  ~Backend();

  void Update();

 public:
  void Pixel(std::int16_t x, std::int16_t y, nc::ARGB colour) {
    b_buffer_[nc::PPU::kFrameW * y + x] = colour;
  }

  bool IsRunning() {
    return running_;
  }

 private:
  void HandleEvents();

 private:
  SDL_Window* window_;

  SDL_Renderer* renderer_;

  SDL_Texture* texture_;

  Uint32* b_buffer_;

  Uint32* f_buffer_;

  const nc::NES& nes_;

  nc::NES::Controller* players_[2];

  bool running_ = true;

  Clock::TimePoint updated_time_ = Clock::Now();
};

#endif  // ifndef _BACKEND_H_
