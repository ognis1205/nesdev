/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_DETAIL_PIPELINE_H_
#define _NES_CORE_DETAIL_PIPELINE_H_
#include <deque>
#include <functional>

namespace nes {
namespace core {
namespace detail {

class Pipeline final {
 public:
  enum class Status { Continue, Skip, Stop };

  using Step = std::function<Status()>;

  Pipeline() = default;

  void Push(const std::function<void()>& step) noexcept;

  void Push(const Step& step) noexcept;

  void Stage(Pipeline& other) noexcept;

  bool Done() const noexcept;

  void Clear() noexcept;

  void Tick();

 private:
  Status status_ = Status::Continue;

  std::deque<Step> steps_ {};
};

}  // namespace detail
}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_DETAIL_PIPELINE_H_
