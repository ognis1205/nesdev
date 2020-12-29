/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <functional>
#include <iterator>
#include "detail/pipeline.h"
#include "macros.h"

namespace nesdev {
namespace core {
namespace detail {

void Pipeline::Push(const std::function<void()>& step) noexcept {
  steps_.emplace_back([step] {
    step();
    return Status::Continue;
  });
}

void Pipeline::Push(const Step& step) noexcept {
  steps_.push_back(std::move(step));
}

void Pipeline::Stage(Pipeline& other) noexcept {
  std::copy(other.steps_.begin(), other.steps_.end(), std::back_inserter(steps_));
}

bool Pipeline::Done() const noexcept {
  return steps_.empty() || status_ == Status::Stop;
}

void Pipeline::Clear() noexcept {
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
