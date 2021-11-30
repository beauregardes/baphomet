#include "hades/util/framecounter.hpp"

#include "hades/util/time.hpp"

namespace hades {

FrameCounter::FrameCounter() {
  start_time_ = hades::time_nsec<std::uint64_t>();
}

void FrameCounter::update() {
  timestamps_.emplace_back(hades::time_nsec<std::uint64_t>());
  while (timestamps_.back() - timestamps_.front() > 1'000'000'000u)
    timestamps_.pop_front();

  averager_.update(timestamps_.size());
  if (ticker_.tick())
    averager_.alpha = 2.0 / (1.0 + timestamps_.size());
}

double FrameCounter::fps() {
  return averager_.value();
}

double FrameCounter::dt() {
  if (timestamps_.size() == 0)
    return 0.0;
  else if (timestamps_.size() == 1)
    return (timestamps_[0] - start_time_) / 1e9;
  else
    return (timestamps_.back() - timestamps_[timestamps_.size() - 2]) / 1e9;
}

} // namespace hades