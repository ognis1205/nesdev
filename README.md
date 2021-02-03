## NesDev Nintendo Entertainment System (NES) Developing Toolkits

![Screenshot of Donkey Kong](https://imgur.com/QETaFxn.png)

### Summary

Modern C++ NES developing toolkits for My Own Learning Porpose. The implementation is NOT fully faithfull to the original NES hardware though,
but CPU instruction implementations are cycle accurate.

### Things NOT faithfull to the original NES hardware so far (01/24/2021)

1. PPU Foregound/Sprites Rendering
 In original NES hardware archetecture, sprite gathering, sprite clearing and sprite evaluation for the next scanline,
natural on the digital circuit, these things are distributed in parallel, but in my implementation, these are embedded
in the PPU cycle along with the background rendering. This may limit compatibility with some games.

### Roadmap

The following is a checklist of features and their progress:
- [x] Console
  - [x] NTSC
  - [ ] PAL
  - [ ] Dendy
  - [ ] Headless mode
- [x] Central Processing Unit (CPU)
  - [x] Official Instructions
  - [ ] Unofficial Instructions (Some still incorrect)
  - [x] Interrupts
- [x] Picture Processing Unit (PPU)
  - [x] VRAM
  - [x] Background
  - [x] Sprites
  - [x] NTSC TV Artifact Effects
  - [x] Emphasize RGB/Grayscale
- [ ] Audio Processing Unit (APU)
  - [ ] Pulse Channels
  - [ ] Triangle Channels
  - [ ] Noise Channels
  - [ ] Delta Mulation Channel (DMC)
- [x] Inputs
  - [x] Keyboard
- [x] Memory
- [x] Cartridge
  - [ ] Battery-backed Save RAM
  - [x] iNES Format
  - [x] NES 2.0 Format (Can read headers, but many features still unsupported)
  - [ ] Mappers
    - [x] NROM (Mapper 0)
    - [ ] SxROM/MMC1 (Mapper 1)
    - [ ] UxROM (Mapper 2)
    - [ ] CNROM (Mapper 3)
    - [ ] TxROM/MMC3 (Mapper 4)
    - [ ] ExROM/MMC5 (Mapper 5) (Split screen and sound is unfinished)
    - [ ] AxROM (Mapper 7)
    - [ ] PxROM/MMC2 (Mapper 9)
- [ ] Testing/Debugging/Documentation
  - [ ] Unit/Integration tests
  - [ ] Test ROMs
  - [ ] Logging

### Generating + Compiling

NesDev builds with **CMake**

On macOS / Linux
```bash
# in NesDev root
mkdir build
cd build
cmake ..
make
make install
```

### Usage

NesDev library (**libnesdev**) is a static library for developing NES emulators, so **libnesdev** it self does NOT
contain any media layer implementations. All you have to do is implememnt media backends with your favorite library,
such as **SDL2**m and hook the NesDev's **Framebuffer** API for PPU and **Sampling** API for APU respectively.

```c++
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

  try {
    std::ifstream ifs(rom, std::ifstream::binary);
    nc::NES nes(nc::ROMFactory::NROM(ifs));
    ifs.close();
    Utility::ShowHeader(nes);

    Backend sdl(nes, nes.controller_1.get(), nes.controller_2.get());
    nes.ppu->Framebuffer([&sdl](std::int16_t x, std::int16_t y, nc::ARGB rgba) {
      sdl.Pixel(x, y, rgba);
    });
    //nes->apu->Sampling([&sdl]() {
    //  /* This is a placeholder for APU API. */
    //});

    if (cli.Defined("--chr_rom")) {
      while (sdl.IsRunning()) {
	Utility::RenderCHRRom(nes, sdl);
	sdl.Update();
      }
    } else {
      while (sdl.IsRunning()) {
	nes.Tick();
	if (/*nes.cpu->IsIdle() &&*/ (nes.cycle % 3 == 0))
	  Utility::Trace(nes);
	if (nes.ppu->IsPostRenderLine() && nes.ppu->Cycle() == 0) {
	  sdl.Update();
	}
      }
    }
  } catch (const std::exception& e) {
    Utility::ShowStackTrace();
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
```

### References

1. [NesDev](http://nesdev.com/)
2. [Bisqwit's implementation](https://bisqwit.iki.fi/jutut/kuvat/programming_examples/nesemu1/)
3. [Javidx9's series of NES emulator](https://www.youtube.com/watch?v=F8kx56OZQhg)
