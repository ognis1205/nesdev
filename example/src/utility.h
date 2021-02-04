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
#include <sstream>
#include <queue>
#include <deque>
#include <string>
#include <nesdev/core.h>
#include "backend.h"

namespace nc = nesdev::core;

namespace {

template <typename T, std::size_t Size, typename Container=std::deque<T>>
class FixedQueue : public std::queue<T, Container> {
 public:
  void push(const T& value) {
    if (this->size() == Size)
      this->c.pop_front();
    std::queue<T, Container>::push(value);
  }
};

FixedQueue<std::string, 100> stack_trace;

}

class Utility {
 public:
  static void Init() {
    srand(time(nullptr));
  }

  template<nc::ARGB From, nc::ARGB To>
  static nc::ARGB Noise() {
    return From + rand() % (To - From + 1);
  }

  static void ShowHeader(const nc::NES& nes) {
    std::cerr << "================" << std::endl;
    std::cerr << "[HEADER]" << std::endl;
    std::cerr << "prg rom:" << nes.rom->header->SizeOfPRGRom() << std::endl;
    std::cerr << "chr rom:" << nes.rom->header->SizeOfCHRRom() << std::endl;
    std::cerr << "prg ram:" << nes.rom->header->SizeOfPRGRam() << std::endl;
    std::cerr << "chr ram:" << nes.rom->header->SizeOfCHRRam() << std::endl;
  }

  static std::string Info(const nc::NES& nes) {
    std::stringstream ss;

    ss << "================" << std::endl;
    ss << "[CPU]" << std::endl;

    ss << std::hex << unsigned(nes.cpu->Op()) << " ";
    ss << nc::Opcodes::ToString(nes.cpu->Op()) << std::endl;

    ss << "      pc: ";
    ss << std::hex << unsigned(nes.cpu->PCRegister()) << std::endl;

    ss << "     acc: ";
    ss << std::hex << unsigned(nes.cpu->ARegister()) << std::endl;

    ss << "       x: ";
    ss << std::hex << unsigned(nes.cpu->XRegister()) << std::endl;

    ss << "       y: ";
    ss << std::hex << unsigned(nes.cpu->YRegister()) << std::endl;

    ss << "    stkp: ";
    ss << std::hex << unsigned(nes.cpu->SRegister()) << std::endl;

    ss << "    addr: ";
    ss << std::hex << unsigned(nes.cpu->Addr()) << std::endl;
    ss << std::endl;

    std::bitset<8> p(nes.cpu->PRegister());
    ss << "czidbuon" << std::endl;
    ss << "aerernfe" << std::endl;
    ss << "rrqckulg" << std::endl;
    ss << p << std::endl;
    ss << std::endl;

    ss << "[PPU]" << std::endl;

    ss << "    vram: ";
    ss << std::hex << unsigned(nes.ppu->VRAMAddr()) << std::endl;

    std::bitset<16> vram(nes.ppu->VRAMAddr());
    ss << "u  fnn    c    c" << std::endl;
    ss << "s  |||    |    |" << std::endl;
    ss << "e  yyx    y    x" << std::endl;
    ss << vram << std::endl;

    ss << "    tram: ";
    ss << std::hex << unsigned(nes.ppu->TRAMAddr()) << std::endl;

    std::bitset<16> tram(nes.ppu->TRAMAddr());
    ss << "u  fnn    c    c" << std::endl;
    ss << "s  |||    |    |" << std::endl;
    ss << "e  yyx    y    x" << std::endl;
    ss << tram << std::endl;

    ss << "   bg_id: ";
    ss << std::hex << unsigned(nes.ppu->BgId()) << std::endl;

    ss << "   bg_at: ";
    ss << std::hex << unsigned(nes.ppu->BgAttr()) << std::endl;

    ss << "   bg_ls: ";
    ss << std::hex << unsigned(nes.ppu->BgLSB()) << std::endl;

    ss << "   bg_ms: ";
    ss << std::hex << unsigned(nes.ppu->BgMSB()) << std::endl;

    std::bitset<16> bg_pttr_lo(nes.ppu->BgPttrLo());
    ss << "bg_pt_lo: " << std::endl;;
    ss << bg_pttr_lo << std::endl;

    std::bitset<16> bg_pttr_hi(nes.ppu->BgPttrHi());
    ss << "bg_pt_hi: " << std::endl;
    ss << bg_pttr_hi << std::endl;
    ss << std::endl;

    std::bitset<8> ppuctrl(nes.ppu->CtrlRegister());
    ss << "nnisbsmn" << std::endl;
    ss << "xyntth|m" << std::endl;
    ss << "  ciiesi" << std::endl;
    ss << ppuctrl << std::endl;
    ss << std::endl;

    std::bitset<8> ppumask(nes.ppu->MaskRegister());
    ss << "gbsbsrgb" << std::endl;
    ss << "rlleeerl" << std::endl;
    ss << "yffnndeu" << std::endl;
    ss << ppumask << std::endl;

    return ss.str();
  }

  static void Trace(const nc::NES& nes) {
    ::stack_trace.push(Info(nes));
  }

  static void ShowStackTrace() {
    while (!::stack_trace.empty()) {
      std::cerr << ::stack_trace.front();
      ::stack_trace.pop();
    }
  }

# define TILE_BYTE_COUNT 16
# define TILE_PIXEL_H 8
# define TILE_PIXEL_W 8
# define TILE_INDEX(x, i) (((x) & (0x1 << (7 - (i)))) >> (7 - (i)))

# define NUM_PTTR_TABLES 2
# define PTTR_TILE_COUNT 256
# define PTTR_BYTE_COUNT (TILE_BYTE_COUNT * PTTR_TILE_COUNT)
# define PTTR_DISP_W 128

# define R(x) ((x) & 0x00FF0000)
# define G(x) ((x) & 0x0000FF00)
# define B(x) ((x) & 0x000000FF)

  static void RenderCHRRom(const nc::NES& nes, Backend& sdl) {
    for (auto h = 0; h < NUM_PTTR_TABLES; h++) {
      for (auto i = 0; i < PTTR_TILE_COUNT; i++) {
        for (auto j = 0; j < TILE_PIXEL_H; j++) {
	  nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j) & 0xFF00) >> 8);
	  nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j) & 0x00FF) >> 0);
	  nes.ppu->Read(0x2007); // Data deffered
	  nc::Byte lsb = nes.ppu->Read(0x2007);

	  nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j + TILE_BYTE_COUNT / 2) & 0xFF00) >> 8);
	  nes.ppu->Write(0x2006, ((h * PTTR_BYTE_COUNT + i * TILE_BYTE_COUNT + j + TILE_BYTE_COUNT / 2) & 0x00FF) >> 0);
	  nes.ppu->Read(0x2007); // Data deffered
	  nc::Byte msb = nes.ppu->Read(0x2007);

	  for (auto k = 0; k < TILE_PIXEL_W; k++) {
	    std::int16_t x = h * PTTR_DISP_W + (i % TILE_BYTE_COUNT) * TILE_PIXEL_W + k;
	    std::int16_t y = (i / TILE_BYTE_COUNT) * TILE_PIXEL_H + j;

            nc::ARGB argb = nes.ppu->Colour(
	      nes.ppu_registers->ppumask.intensity,
	      ((1 & 0x3) << 2) | (TILE_INDEX(msb, k) << 1) | TILE_INDEX(lsb, k));
	    nc::Byte grey = 0.2126f * R(argb) + 0.7152f * G(argb) + 0.0722f * B(argb);
	    sdl.Pixel(x, y, grey << 16 | grey << 8 | grey);
	  }
	}
      }
    }
  }

 private:
  Utility();
};

# undef TILE_BYTE_COUNT
# undef TILE_PIXEL_H
# undef TILE_PIXEL_W
# undef TILE_INDEX

# undef NUM_PTTR_TABLES
# undef PTTR_TILE_COUNT
# undef PTTR_BYTE_COUNT
# undef PTTR_DISP_W

# undef R
# undef G
# undef B

#endif  // ifndef _UTILITY_H_
