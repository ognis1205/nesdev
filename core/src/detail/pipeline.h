/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_DETAIL_PIPELINE_H_
#define _NESDEV_CORE_DETAIL_PIPELINE_H_
#include <deque>
#include <functional>
#include "nesdev/core/macros.h"

namespace nesdev {
namespace core {
namespace detail {

class Pipeline final {
 public:
  enum class Status { Continue, Skip, Stop };

  using Step = std::function<Status()>;

  Pipeline() = default;

  void Push(const std::function<void()>& step);

  void Push(const Step& step);

  void Append(const Pipeline& other);

  bool Done() const;

  void Clear();

  void Tick();

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  Status status_ = Status::Continue;

  std::deque<Step> steps_ = {};
};

}  // namespace detail
}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_DETAIL_PIPELINE_H_
