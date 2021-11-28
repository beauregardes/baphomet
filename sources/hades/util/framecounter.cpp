#include "hades/util/framecounter.hpp"

#include "hades/util/time.hpp"

namespace hades {

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

} // namespace hades