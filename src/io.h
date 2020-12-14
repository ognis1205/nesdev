/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "types.h"

namespace nes {
namespace io {

/* NES Resolution Width. */
const size_t kNESResolutionWidth = 256;

/* NES Resolution Width. */
const size_t kNESResolutionHeight = 240;


/* Initialize SDL. */
void Init();

/* Put/Draw 'pixel' at position ('x', 'y') with specified 'offset'. */
void PutPixel(unsigned x, unsigned y, unsigned pixel, int offset);

/*
 * On hardware that supports double-buffering, this function sets up a flip and
 * returns. This must be occured at visible-scanlaine #239.
 * SEE: https://wiki.nesdev.com/w/index.php/PPU_rendering
 */
void FlushScanline(unsigned y);

/* Flag that joystick controller input is allowed specified with value 'v'. */
void StrobeJoystick(unsigned v);

/* Read joystick controller specified with index 'i'. */
types::uint8_t ReadJoystick(unsigned i);

}  // namespace io
}  // namespace nes
