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
#include <SDL_ttf.h>
#include <nesdev/core.h>
#include "backend.h"
#include "cli.h"
#include "utility.h"

namespace nc = nesdev::core;

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
    nc::NES nes(nc::ROMFactory::NROM(ifs));
    Backend sdl(nes.controller_1.get(), nes.controller_2.get());
    ifs.close();

    nes.ppu->Framebuffer([&sdl](std::int16_t x, std::int16_t y, [[maybe_unused]]nc::RGBA rgba) {
      sdl.Pixel(x, y, rgba);
    });

    //nes->apu->Sampling([]() {
    //  /* This is a placeholder for APU API. */
    //});

    while (sdl.IsRunning()) {
      nes.Tick();
      if (nes.cpu->Idle() && (nes.cycle % 3 == 0)) {
	Utility::Debug(nes);
	int test;
	std::cin >> test;
      }
      if (nes.ppu->IsPostRenderLine() && nes.ppu->Cycle() == 0) {
	sdl.Update();
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
