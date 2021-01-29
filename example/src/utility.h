/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <nesdev/core.h>

namespace nc = nesdev::core;

class Utility {
 public:
  static void Init() noexcept {
    srand(time(nullptr));
  }

  template<nc::RGBA From, nc::RGBA To>
  static nc::RGBA Noise() noexcept {
    return From + rand() % (To - From + 1);
  }

  static void Debug(const nc::NES& nes) noexcept {
    std::cerr << "[[[OPCODE]]]" << std::endl;
    std::cerr << std::hex << unsigned(nes.Opcode()) << " ";
    std::cerr << nc::Opcodes::ToString(nes.Opcode()) << std::endl;
    std::cerr << "  pc: ";
    std::cerr << std::hex << unsigned(nes.ProgramCounter()) << std::endl;
    std::cerr << " acc: ";
    std::cerr << std::hex << unsigned(nes.Accumulator()) << std::endl;
    std::cerr << "   x: ";
    std::cerr << std::hex << unsigned(nes.XRegister()) << std::endl;
    std::cerr << "   y: ";
    std::cerr << std::hex << unsigned(nes.YRegister()) << std::endl;
    std::cerr << "stkp: ";
    std::cerr << std::hex << unsigned(nes.StackPointer()) << std::endl;
    std::cerr << "addr: ";
    std::cerr << std::hex << unsigned(nes.EffectiveAddress()) << std::endl;
    std::cerr << std::endl;
    std::cerr << "czidbuon" << std::endl;
    std::cerr << "aerernfe" << std::endl;
    std::cerr << "rrqckulg" << std::endl;
    std::bitset<8> p(nes.StatusRegister());
    std::cerr << p << std::endl;
    std::cerr << std::endl;
    std::cerr << "[[[PPU]]]" << std::endl;
    std::cerr << "nnisbsmn" << std::endl;
    std::cerr << "xyntth|m" << std::endl;
    std::cerr << "  ciiesi" << std::endl;
    std::bitset<8> ppuctrl(nes.PPUControlRegister());
    std::cerr << ppuctrl << std::endl;
//	std::cout << "    grey: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.greyscale) << std::endl;
//	std::cout << "   bleft: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.background_enable) << std::endl;
//	std::cout << "   sleft: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.sprite_leftmost_enable) << std::endl;
//	std::cout << " benable: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.background_enable) << std::endl;
//	std::cout << " senable: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.sprite_enable) << std::endl;
//	std::cout << "  intent: ";
// 	std::cout << std::hex << unsigned(nes.ppu_registers->ppumask.intensity) << std::endl;
//	std::cout << std::endl;
//	std::cout << "vramaddr: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->vramaddr.value) << std::endl;
//	std::cout << "tramaddr: ";
//	std::cout << std::hex << unsigned(nes.ppu_registers->tramaddr.value) << std::endl;
  }

 private:
  Utility();
};

#endif  // ifndef _UTILITY_H_
