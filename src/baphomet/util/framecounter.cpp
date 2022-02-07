#include "baphomet/util/framecounter.hpp"

using namespace std::chrono_literals;

namespace baphomet {

/* TODO: Refactor this to use Duration instead of doing times as doubles
 *   so it'll match the rest of the library
 */

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T>
T time_nsec() {
  using namespace std::chrono;

  auto now = steady_clock::now().time_since_epoch();
  return static_cast<T>(duration_cast<nanoseconds>(now).count());
}

FrameCounter::FrameCounter() {
  start_time_ = time_nsec<std::uint64_t>();
}

void FrameCounter::update() {
  timestamps_.emplace_back(time_nsec<std::uint64_t>());
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
  if (timestamps_.empty())
    return 0.0;
  else if (timestamps_.size() == 1)
    return (timestamps_[0] - start_time_) / 1e9;
  else
    return (timestamps_.back() - timestamps_[timestamps_.size() - 2]) / 1e9;
}

} // namespace baphomet