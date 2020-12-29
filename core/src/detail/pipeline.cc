/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <functional>
#include <iterator>
#include "nesdev/core/macros.h"
#include "detail/pipeline.h"

namespace nesdev {
namespace core {
namespace detail {

void Pipeline::Push(const std::function<void()>& step) NESDEV_CORE_NOEXCEPT {
  steps_.emplace_back([step] {
    step();
    return Status::Continue;
  });
}

void Pipeline::Push(const Step& step) NESDEV_CORE_NOEXCEPT {
  steps_.push_back(std::move(step));
}

void Pipeline::Stage(Pipeline& other) NESDEV_CORE_NOEXCEPT {
  std::copy(other.steps_.begin(), other.steps_.end(), std::back_inserter(steps_));
}

bool Pipeline::Done() NESDEV_CORE_CONST NESDEV_CORE_NOEXCEPT {
  return steps_.empty() || status_ == Status::Stop;
}

void Pipeline::Clear() NESDEV_CORE_NOEXCEPT {
  while (!steps_.empty()) {
    steps_.pop_front();
  }
  status_ = Status::Continue;
}

void Pipeline::Tick() {
  if (!steps_.empty()) {
    status_ = steps_.front()();
    steps_.pop_front();
    if (status_ == Status::Skip) Tick();
  }
}

}  // namespace detail
}  // namespace core
}  // namespace nesdev
