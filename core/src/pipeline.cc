/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#include <functional>
#include <iterator>
#include "pipeline.h"

namespace nes {
namespace core {

void Pipeline::Stage(const std::function<void()>& step) noexcept {
  steps_.emplace_back([step] {
    step();
    return Status::Continue;
  });
}

void Pipeline::Stage(const Step& step) noexcept {
  steps_.push_back(std::move(step));
}

void Pipeline::AppendTo(Pipeline& other) noexcept {
  std::copy(steps_.begin(), steps_.end(), std::back_inserter(other.steps_));
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

}  // namespace core
}  // namespace nes
