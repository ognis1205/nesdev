/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <vector>
#include "nesdev/core/palettes.h"
#include "nesdev/core/types.h"

namespace {

using namespace nesdev::core;

/*
 * NOTE:
 * The following instructions are defined according to these references:
 * [SEE] https://www.masswerk.at/6502/6502_instruction_set.html
 * [SEE] https://undisbeliever.net/snesdev/65816-opcodes.htm
 * [SEE] http://nparker.llx.com/a2/opcodes.html
 */
const std::vector<Byte> pl2c02 = {

};

}

namespace nesdev {
namespace core {

std::vector<Byte> Decode([[maybe_unused]] Palette palette) noexcept {
  return std::vector<Byte>(0);
}

}  // namespace core
}  // namespace nesdev
