/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _BACKEND_H_
#define _BACKEND_H_
#include <cstddef>
#include <nesdev/core.h>
#include <SDL.h>

namespace nc = nesdev::core;

class Backend {
 public:
  static const int kFPS = 60;

  static constexpr int kDelay = 1000.0f / kFPS;

 public:
  Backend(nc::NES::Controller* const player_one, nc::NES::Controller* const player_two);

  ~Backend();

  void Draw();

  void Run();

  void HandleEvents();

  void Stop();

 public:
  void Pixel(std::int16_t x, std::int16_t y, nc::RGBA colour) {
    b_buffer_[nc::PPU::kFrameW * y + x] = colour;
  }

  bool IsRunning() {
    return running_;
  }

 private:
  SDL_Window* window_;

  SDL_Renderer* renderer_;

  SDL_Texture* texture_;

  Uint32* b_buffer_;

  Uint32* f_buffer_;

  SDL_mutex* frame_mutex_;

  SDL_mutex* event_mutex_;

  SDL_cond* frame_condition_;

  bool running_ = true;

  bool ready_to_draw_ = false;

  bool frame_available_ = false;

  bool pending_thread_exit_ = false;

  nc::NES::Controller* players_[2];
};

#endif  // ifndef _BACKEND_H_
