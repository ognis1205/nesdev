# Nintendo Entertainment System (NES) Emulator

C++ NES Implementation for My Own Learning Porpose.

The implementation is NOT fully faithfull to the original NES hardware.

# Archetecture

```
                        +-------------+   +---------------+   +---------------+   +------------------------------+
                        |             |   |               |   |               |   |                   +--------+ |
          +-------------+ CPU (2AO3)  |   | MEMORY (2KB)  |   | APU           |   | CARTRIGDE         | MAPPER | |
          |             |             |   | $0000 - $07FF |   | $4000 - $4017 |   | $4020 - $FFFF  +--+--------+ |
          |   +---------++------------+   +---------------+   +--------------     ++-----------------------------+
          |   |         ||                |                   |                   ||               |
          |   |         |+----------------|-------------------|-------------------||---------------+
  +-------+   |         |                 |                   |                   ||
  | CLOCK |   |         +-----------------+-------------------+-------------------+------------------------------> BUS
  +-------+   +-----+   |                   +- - - - - - - - - - - - - - - - - - - +
          |   | DMA |   |                   |
          |   +-----+   |                   |
          |   |         +---------------+   +----------------+   +---------------+   +---------------+
          |   |         | $2000 - $2007 |   |                |   |               |   |               |
          +-------------+ PPU (2CO2)    |   | GRAPHICS (8KB) |   | VRAM (2KB)    |   | PALETTES      |
              |         |               |   | $0000 - $1FFF  |   | $2000 - $27FF |   | $3F00 - $3FFF |
              +---------+---------------+   +----------------+   +---------------+   +---------------+
              |         |                   |                    |                   |
              +-----+   |                   |                    |                   |
              | OAM |   |                   |                    |                   |
              +-----+   |                   |                    |                   |
                        +-------------------+--------------------+-------------------+---------------------------> BUS
```

# References

1. [NesDev](http://nesdev.com/)
2. [Bisqwit's implementation](https://bisqwit.iki.fi/jutut/kuvat/programming_examples/nesemu1/)
3. [Javidx9's series of NES emulator](https://www.youtube.com/watch?v=F8kx56OZQhg)

