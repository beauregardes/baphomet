#pragma once

#include "baphomet/util/time/ticker.hpp"
#include "baphomet/util/averagers.hpp"

#include <cstdint>
#include <deque>

namespace baphomet {

class FrameCounter {
public:
  FrameCounter();

  void update();

  double fps();
  double dt();

private:
  std::uint64_t start_time_{};
  std::deque<std::uint64_t> timestamps_{};

  Ticker ticker_{std::chrono::milliseconds(500)};
  EMA averager_{1.0};
};

} // namespace baphomet
