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
  if ((rom = std::filesystem::absolute(cli.Get("--rom"))).empty()) {
    std::cerr << "iNES file must be specified" << std::endl;
    exit(1);
  }

    std::ifstream ifs(rom, std::ifstream::binary);
    nc::NES nes(nc::ROMFactory::NROM(ifs));
    ifs.close();

    Backend sdl(nes, nes.controller_1.get(), nes.controller_2.get());
    nes.ppu->Framebuffer([&sdl](std::int16_t x, std::int16_t y, nc::ARGB rgba) {
      sdl.Pixel(x, y, rgba);
    });


  try {
    if (cli.Defined("--chr_rom")) {
      while (sdl.IsRunning()) {
	Utility::RenderCHRRom(nes, sdl);
	sdl.Update();
      }
    } else {
      while (sdl.IsRunning()) {
	nes.Tick();
	if (nes.ppu->IsPostRenderLine() && nes.ppu->Cycle() == 0)
	  sdl.Update();
      }
    }
  } catch (std::exception& e) {
    std::cerr << Utility::Info(nes) << std::endl;
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
