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
  Duration dt();

private:
  Clock::time_point start_time_{};
  std::deque<Clock::time_point> timestamps_{};

  Ticker ticker_{std::chrono::milliseconds(500)};
  EMA averager_{1.0};
};

} // namespace baphomet
