#include "baphomet/util/time/ticker.hpp"

namespace baphomet {

Ticker::Ticker(Duration interval)
    : start_time_(Clock::now()), last_time_(Clock::now()), interval_(interval) {}

bool Ticker::tick() {
  auto now = Clock::now();
  dt_ = now - last_time_;
  last_time_ = now;

  if (interval_ > Duration(0.0)) {
    acc_ += dt_;
    if (acc_ > interval_) {
      while (acc_ > interval_)
        acc_ -= interval_;
      return true;
    }
  }
  return false;
}

Duration Ticker::total_elapsed_dt() const {
  return last_time_ - start_time_;
}

Duration Ticker::dt() const {
  return dt_;
}

} // namespace baphomet
