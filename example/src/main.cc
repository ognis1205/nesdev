/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <SDL.h>
#include <nesdev/core.h>
#include "cli.h"

namespace fs = std::filesystem;
namespace nc = nesdev::core;

int main(int argc, char** argv) {
  CLI cli(argc, argv);

  std::string rom;
  if ((rom = fs::absolute(cli.Get("-f"))).empty())
    throw std::runtime_error("iNES file must be specified");

  std::ifstream ifs(rom, std::ifstream::binary);
  nc::NES nes(nc::ROMFactory::NROM(ifs));

//  SDL_Init(SDL_INIT_VIDEO);
//  SDL_Window* window = SDL_CreateWindow(
//    "NES SDL2",
//    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, nc::PPU::kFrameW, nc::PPU::kFrameH, 0);
//  SDL_Renderer* renderer = SDL_CreateRenderer(
//    window,
//    -1, 0);
//  SDL_Texture* texture = SDL_CreateTexture(
//    renderer,
//    SDL_PIXELFORMAT_ARGB8888,
//    SDL_TEXTUREACCESS_STREAMING,
//    nc::PPU::kFrameW,
//    nc::PPU::kFrameH);
  int pitch;
//  void *pixels = nullptr;
//
//  SDL_LockTexture(texture, NULL, &pixels, &pitch);
//  memset(pixels, 255, nc::PPU::kFrameW * nc::PPU::kFrameH * sizeof(Uint32));
//  SDL_UnlockTexture(texture);
//  SDL_RenderCopy(renderer, texture, NULL, NULL);
//  SDL_RenderPresent(renderer);
//
//  SDL_Event event;
  int frame = 0;
  while (true) {
//    SDL_LockTexture(texture, NULL, &pixels, &pitch);
//    memset(pixels, 255, nc::PPU::kFrameW * nc::PPU::kFrameH * sizeof(Uint32));

    nes.ppu->Framebuffer([/*pixels*/&pitch]([[maybe_unused]]std::int16_t x, [[maybe_unused]]std::int16_t y, [[maybe_unused]]nc::RGBA rgba) {
//      static_cast<nc::RGBA*>(pixels)[scanline * nc::PPU::kFrameW + cycle] = rgba;
      pitch = rgba;
    });
    nes.Tick();

    if (nes.ppu->Scanline() == 0) std::cout << frame++ << " " << pitch << std::endl;
//    if (nes.ppu->Scanline() == 0) std::cout << frame++ << std::endl;

//    SDL_UnlockTexture(texture);
//    SDL_RenderCopy(renderer, texture, NULL, NULL);
//    SDL_RenderPresent(renderer);
//
//    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
  }

//  SDL_DestroyTexture(texture);
//  SDL_DestroyRenderer(renderer);
//  SDL_DestroyWindow(window);
//  SDL_Quit();

  return EXIT_SUCCESS;
}
