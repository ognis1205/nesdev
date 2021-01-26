/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <iostream>
#include <memory>
#include "nesdev/core.h"
#include "cli.h"

int main(int argc, char** argv) {
  CLI cli(argc, argv);
  std::cout << cli.Defined("-t") << std::endl;
  std::cout << cli.Get("-f") << std::endl;
  std::cout << "Hello, Nesdev!" << std::endl;
}
