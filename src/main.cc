/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <iostream>
#include "SDL.h"
#include "io.h"

int main(int argc, char* argv[]) {
  nes::io::NTSCDisplay display;

  if (!display) {
    std::cerr << "Could not create display: " << SDL_GetError() << std::endl;
    return 1;
  }

  return 0;
}
