#include "baphomet/util/framecounter.hpp"

using namespace std::chrono_literals;

namespace baphomet {

FrameCounter::FrameCounter() {
  start_time_ = Clock::now();
}

void FrameCounter::update() {
  timestamps_.emplace_back(Clock::now());
  while (timestamps_.back() - timestamps_.front() > 1s)
    timestamps_.pop_front();

  averager_.update(timestamps_.size());
  if (ticker_.tick())
    averager_.alpha = 2.0 / (1.0 + timestamps_.size());
}

double FrameCounter::fps() {
  return averager_.value();
}

Duration FrameCounter::dt() {
  if (timestamps_.empty())
    return Duration(0);
  else if (timestamps_.size() == 1)
    return timestamps_[0] - start_time_;
  else
    return timestamps_.back() - timestamps_[timestamps_.size() - 2];
}

} // namespace baphomet