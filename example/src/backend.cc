/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <nesdev/core.h>
#include <SDL.h>
#include "backend.h"

namespace nc = nesdev::core;

Backend::Backend(nc::NES::Controller* const player_one,
                 nc::NES::Controller* const player_two) {
  players_[0] = player_one;
  players_[1] = player_two;
  if (SDL_Init(SDL_INIT_VIDEO
	       | SDL_INIT_AUDIO
	       | SDL_INIT_JOYSTICK
	       | SDL_INIT_GAMECONTROLLER) != 0) {
    std::stringstream ss("failed to initialize SDL: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  if (!(window_ = SDL_CreateWindow(NULL,
				   SDL_WINDOWPOS_CENTERED,
				   SDL_WINDOWPOS_CENTERED,
				   nc::PPU::kFrameW,
				   nc::PPU::kFrameH,
				   SDL_WINDOW_RESIZABLE))) {
    std::stringstream ss("failed to create window: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  if(!(renderer_ = SDL_CreateRenderer(window_,
				      -1,
				      SDL_RENDERER_TARGETTEXTURE))) {
    std::stringstream ss("failed to create rendering context: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  if (!(texture_ = SDL_CreateTexture(renderer_,
				     SDL_PIXELFORMAT_ARGB8888,
				     SDL_TEXTUREACCESS_STREAMING,
				     nc::PPU::kFrameW,
				     nc::PPU::kFrameH))) {
    std::stringstream ss("failed to create texture for screen: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  static Uint32 buffers[2][nc::PPU::kFrameW * nc::PPU::kFrameH];
  b_buffer_ = buffers[0];
  f_buffer_ = buffers[1];

  SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONUP,   SDL_IGNORE);
  SDL_EventState(SDL_MOUSEMOTION,     SDL_IGNORE);
  SDL_EventState(SDL_WINDOWEVENT,     SDL_IGNORE);

  if (!(event_mutex_ = SDL_CreateMutex())) {
    std::stringstream ss("failed to create event mutex: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  if (!(frame_mutex_ = SDL_CreateMutex())) {
    std::stringstream ss("failed to create frame mutex: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }

  if(!(frame_condition_ = SDL_CreateCond())) {
    std::stringstream ss("failed to create frame condition variable: "); ss << SDL_GetError();
    throw std::runtime_error(ss.str());
  }
}

Backend::~Backend() {
  if (renderer_)        SDL_DestroyRenderer(renderer_);
  if (window_)          SDL_DestroyWindow(window_);
  if (event_mutex_)     SDL_DestroyMutex(event_mutex_);
  if (frame_mutex_)     SDL_DestroyMutex(frame_mutex_);
  if (frame_condition_) SDL_DestroyCond(frame_condition_);
  SDL_QuitSubSystem( SDL_INIT_GAMECONTROLLER );
  SDL_Quit();
}

void Backend::Draw() {
  SDL_LockMutex(frame_mutex_);
  std::uint32_t start = SDL_GetTicks();
  if (ready_to_draw_) {
    frame_available_ = true;
    std::swap(b_buffer_, f_buffer_);
    SDL_CondSignal(frame_condition_);
  }
  SDL_UnlockMutex(frame_mutex_);

  std::uint32_t delay = SDL_GetTicks() - start;
  if (delay < Backend::kDelay) {
    SDL_Delay((int)(Backend::kDelay - delay));
  }
}

void Backend::Run() {
  SDL_UnlockMutex(frame_mutex_);
  for (;;) {
    SDL_RenderClear(renderer_);

    ready_to_draw_ = true;

    while (!frame_available_ && !pending_thread_exit_)
      SDL_CondWait(frame_condition_, frame_mutex_);

    if (pending_thread_exit_) {
      SDL_UnlockMutex(frame_mutex_);
      pending_thread_exit_ = false;
      return;
    }

    frame_available_ = ready_to_draw_ = false;
    HandleEvents();

    if (SDL_UpdateTexture(texture_, 0, f_buffer_, nc::PPU::kFrameW * sizeof(Uint32))) {
      std::stringstream ss("failed to update screen texture: "); ss << SDL_GetError();
      throw std::runtime_error(ss.str());
    }

    if(SDL_RenderCopy(renderer_, texture_, 0, 0)) {
      std::stringstream ss("failed to copy rendered frame to render target: "); ss << SDL_GetError();
      throw std::runtime_error(ss.str());
    }
    SDL_RenderPresent(renderer_);
  }
}

void Backend::HandleEvents() {
  SDL_Event event;
  SDL_LockMutex(event_mutex_);
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      pending_thread_exit_ = true;
      running_ = false;
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_RETURN:    std::cout << "RET" << std::endl; players_[0]->Start(true);  break;
      case SDLK_BACKSPACE: std::cout << "BAK" << std::endl; players_[0]->Select(true); break;
      case SDLK_UP:        std::cout << "↑" << std::endl; players_[0]->Up(true);     break;
      case SDLK_DOWN:      std::cout << "↓" << std::endl; players_[0]->Down(true);   break;
      case SDLK_LEFT:      std::cout << "←" << std::endl; players_[0]->Left(true);   break;
      case SDLK_RIGHT:     std::cout << "→" << std::endl; players_[0]->Right(true);  break;
      case SDLK_z:         std::cout << "B" << std::endl; players_[0]->B(true);      break;
      case SDLK_x:         std::cout << "A" << std::endl; players_[0]->A(true);      break;
      default: break;
      }
      break;
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_RETURN:    players_[0]->Start(false);  break;
      case SDLK_BACKSPACE: players_[0]->Select(false); break;
      case SDLK_UP:        players_[0]->Up(false);     break;
      case SDLK_DOWN:      players_[0]->Down(false);   break;
      case SDLK_LEFT:      players_[0]->Left(false);   break;
      case SDLK_RIGHT:     players_[0]->Right(false);  break;
      case SDLK_z:         players_[0]->B(false);      break;
      case SDLK_x:         players_[0]->A(false);      break;
      default: break;
      }
      break;
    default:
      break;
    }
  }
  SDL_UnlockMutex(event_mutex_);
}

void Backend::Stop() {
  SDL_LockMutex(frame_mutex_);
  pending_thread_exit_ = true;
  SDL_CondSignal(frame_condition_);
  SDL_UnlockMutex(frame_mutex_);
}
