/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <SDL.h>
#include <nesdev/core.h>
#include "backend.h"
#include "cli.h"

namespace fs = std::filesystem;
namespace nc = nesdev::core;

nc::NES* nes;
Backend* sdl;

int Emulation(void*) {
  for (;;) nes->Tick();
  return 0;
}

void Render() {
  SDL_Thread* emulator;
//  SDL_Thread* m_thread;

//  exitFlag = false;
//  running_state = true;
  if (!(emulator = SDL_CreateThread(Emulation, "emulation", 0)))
    exit(1);

  while (true) {
    sdl->Run();
    SDL_WaitThread(emulator, 0);
//    if (exitFlag) {
//      exitFlag = false;
//      return;
//    }
  }
}

int main(int argc, char** argv) {
  CLI cli(argc, argv);

  std::string rom;
  if ((rom = fs::absolute(cli.Get("-f"))).empty())
    throw std::runtime_error("iNES file must be specified");

  std::ifstream ifs(rom, std::ifstream::binary);
  nes = new nc::NES(nc::ROMFactory::NROM(ifs));
  sdl = new Backend(nes->controller_1.get(), nes->controller_2.get());
  ifs.close();

  nes->ppu->Framebuffer([](std::int16_t x, std::int16_t y, nc::RGBA rgba) {
    sdl->Pixel(x, y, rgba);
  });

  while (sdl->IsRunning()) {
    SDL_ShowCursor(SDL_DISABLE);
    Render();
  }

  delete nes;
  delete sdl;

  return EXIT_SUCCESS;
}
