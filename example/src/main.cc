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
#include "utility.h"

namespace nc = nesdev::core;

nc::NES* nes;
Backend* sdl;

int Emulate(void*) {
  while (sdl->IsRunning()) {
    nes->Tick();
//    std::cout << unsigned(nes->cpu_registers->a.value) << std::endl;
//    std::cout << unsigned(nes->ppu_registers->vramaddr.value) << std::endl;
    if (nes->ppu->IsPostRenderLine() && !nes->ppu->Cycle())
      sdl->Draw();
  }
  return 0;
}

void Start() {
  SDL_Thread* emulation;
  if (!(emulation = SDL_CreateThread(Emulate, "NesDev game engine", 0)))
    exit(1);

  while (sdl->IsRunning()) {
    sdl->Run();
    SDL_WaitThread(emulation, 0);
  }
}

int main(int argc, char** argv) {
  Utility::Init();
  CLI cli(argc, argv);

  std::string rom;
  if ((rom = std::filesystem::absolute(cli.Get("-f"))).empty()) {
    std::cerr << "iNES file must be specified" << std::endl;
    exit(1);
  }

  try {
    std::ifstream ifs(rom, std::ifstream::binary);
    nes = new nc::NES(nc::ROMFactory::NROM(ifs));
    sdl = new Backend(nes->controller_1.get(), nes->controller_2.get());
    ifs.close();

    nes->ppu->Framebuffer([](std::int16_t x, std::int16_t y, [[maybe_unused]]nc::RGBA rgba) {
//      sdl->Pixel(x, y, rgba);
      sdl->Pixel(x, y, Utility::Noise<0x00000000, 0x00FFFFFF>());
    });

    SDL_ShowCursor(SDL_DISABLE);
    Start();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  if (nes) delete nes;
  if (sdl) delete sdl;

  return EXIT_SUCCESS;
}
