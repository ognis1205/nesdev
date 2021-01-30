/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <nesdev/core.h>
#include "backend.h"

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

  static void ShowHeaderInfo(const nc::NES& nes) noexcept {
    std::cerr << "================" << std::endl;
    std::cerr << "[HEADER]" << std::endl;
    std::cerr << "prg rom:" << nes.rom->header->SizeOfPRGRom() << std::endl;
    std::cerr << "chr rom:" << nes.rom->header->SizeOfCHRRom() << std::endl;
    std::cerr << "prg ram:" << nes.rom->header->SizeOfPRGRam() << std::endl;
    std::cerr << "chr ram:" << nes.rom->header->SizeOfCHRRam() << std::endl;
  }

  static void Debug(const nc::NES& nes) noexcept {
    std::cerr << "================" << std::endl;
    std::cerr << "[CPU]" << std::endl;
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
    std::cerr << "[PPU]" << std::endl;
    std::cerr << "vram: ";
    std::cerr << std::hex << unsigned(nes.VRAMAddress()) << std::endl;
    std::cerr << "tram: ";
    std::cerr << std::hex << unsigned(nes.TRAMAddress()) << std::endl;
    std::cerr << std::endl;
    std::cerr << "nnisbsmn" << std::endl;
    std::cerr << "xyntth|m" << std::endl;
    std::cerr << "  ciiesi" << std::endl;
    std::bitset<8> ppuctrl(nes.PPUControl());
    std::cerr << ppuctrl << std::endl;
    std::cerr << std::endl;
    std::cerr << "gbsbsrgb" << std::endl;
    std::cerr << "rlleeerl" << std::endl;
    std::cerr << "yffnndeu" << std::endl;
    std::bitset<8> ppumask(nes.PPUMask());
    std::cerr << ppumask << std::endl;
  }

# define TILE_BYTE_COUNT 16
# define TILE_PIXEL_H 8
# define TILE_PIXEL_W 8
# define TILE_INDEX(x, i) (((x) & (0x1 << (7 - (i)))) >> (7 - (i)))

# define NUM_PTTR_TABLES 2
# define PTTR_TILE_COUNT 256
# define PTTR_BYTE_COUNT (TILE_BYTE_COUNT * PTTR_TILE_COUNT)
# define PTTR_DISP_W 128

  static void RenderCHRRom(const nc::NES& nes, Backend& sdl) noexcept {
    auto palette = nc::Palettes::RP2C02();
    for (auto h = 0; h < NUM_PTTR_TABLES; h++) {
      for (auto i = 0; i < PTTR_TILE_COUNT; i++) {
        for (auto j = 0; j < TILE_PIXEL_H; j++) {
          for (auto k = 0; k < TILE_PIXEL_W; k++) {
	    std::int16_t x = h * PTTR_DISP_W + (i % TILE_BYTE_COUNT) * TILE_PIXEL_W + k;
	    std::int16_t y = (i / TILE_BYTE_COUNT) * TILE_PIXEL_H + j;

            nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j) & 0xFF00) >> 8);
	    nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j) & 0x00FF) >> 0);
	    nes.ppu->Read(0x2007); // Data deffered
	    nc::Byte lo = nes.ppu->Read(0x2007);

            nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j + TILE_BYTE_COUNT / 2) & 0xFF00) >> 8);
	    nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j + TILE_BYTE_COUNT / 2) & 0x00FF) >> 0);
	    nes.ppu->Read(0x2007); // Data deffered
	    nc::Byte hi = nes.ppu->Read(0x2007);

//	    nes.ppu_registers->ppumask.emphasize_red   = true;
//	    nes.ppu_registers->ppumask.emphasize_green = true;
//	    nes.ppu_registers->ppumask.emphasize_blue  = true;
	    nc::RGBA rgba = nes.ppu->Colour(
	      nes.ppu_registers->ppumask.intensity,
	      (TILE_INDEX(hi, k) << 5) | TILE_INDEX(lo, k));
//	      (TILE_INDEX(hi, k) << 1) | TILE_INDEX(lo, k));

	    sdl.Pixel(x, y, rgba);
	  }
	}
      }
    }
  }

 private:
  Utility();
};

#endif  // ifndef _UTILITY_H_
