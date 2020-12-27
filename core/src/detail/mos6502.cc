/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include "nes/core/cpu.h"
#include "nes/core/opcodes.h"
#include "nes/core/types.h"
#include "pipeline.h"

namespace nes {
namespace core {
namespace detail {

class MOS6502 : public CPU {
};

}  // namespace detail
}  // namespace core
}  // namespace nes
