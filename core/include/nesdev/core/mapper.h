/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MAPPER_H_
#define _NESDEV_CORE_MAPPER_H_
#include "nesdev/core/ines_header.h"
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

class Mapper {
 public:
  enum class Space : Byte {
    CPU,
    PPU
  };

 public:
  explicit Mapper(const INESHeader& header)
    : header_{header} {};

  virtual ~Mapper() = default;

  [[nodiscard]]
  virtual bool HasValidAddress(Space space, Address address) const noexcept = 0;

  [[nodiscard]]
  virtual Address MapR(Space space, Address address) const = 0;

  [[nodiscard]]
  virtual Address MapW(Space space, Address address) const = 0;

  virtual void Reset() noexcept = 0;

  [[nodiscard]]
  virtual Mirroring Mirror() const noexcept = 0;

  [[nodiscard]]
  virtual bool IRQ() const noexcept = 0;

  virtual void ClearIRQ() noexcept = 0;

  virtual void Scanline() noexcept = 0;

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  const INESHeader& header_;
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_MAPPER_H_
