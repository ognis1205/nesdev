/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <cstdlib>
#include "SDL.h"
#include "types.h"

namespace nes {
namespace io {

class NTSCDisplay {
 public:
  static const size_t kWidth = 256;
  static const size_t kHeight = 240;
  static const size_t kPixelBits = 9;
  NTSCDisplay();
  ~NTSCDisplay();
  void Draw(size_t px, size_t py, size_t ppu_pixel, int signal_phase);
  void Flush(size_t scanline);
  operator bool();
 private:
  types::uint32_t& GetPixelAt(size_t px, size_t py);
  void InitColorPalette();
  SDL_Window *window_;
  unsigned palette_table_[3][64][1u << kPixelBits];
  unsigned previous_pixel_;
};

class Joystick {
public:
  void Strobe(unsigned v);
  types::uint8_t Read(unsigned i);
};

///*
// * On hardware that supports double-buffering, this function sets up a flip and
// * returns. This must be occured at visible-scanlaine #239.
// * SEE: https://wiki.nesdev.com/w/index.php/PPU_rendering
// */
//
//
///* Flag that joystick controller input is allowed specified with value 'v'. */
//void StrobeJoystick(unsigned v);
//
///* Read joystick controller specified with index 'i'. */
//types::uint8_t ReadJoystick(unsigned i);

}  // namespace io
}  // namespace nes
