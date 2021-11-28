#include "hades/util/ticker.hpp"

#include "hades/util/time.hpp"

namespace hades {

Ticker::Ticker(double interval) {
  start_ = time_nsec<std::uint64_t>();
  last_ = start_;
  interval_ = static_cast<uint64_t>(interval * 1e9);
}

bool Ticker::tick() {
  auto now = time_nsec<std::uint64_t>();
  dt_ = now - last_;
  last_ = now;

  acc_ += dt_;
  if (acc_ >= interval_) {
    acc_ -= interval_;
    return true;
  }
  return false;
}

double Ticker::dt_sec() const {
  return static_cast<double>(dt_ / 1e9);
}

double Ticker::dt_usec() const {
  return static_cast<double>(dt_ / 1e6);
}

double Ticker::dt_msec() const {
  return static_cast<double>(dt_ / 1e3);
}

double Ticker::dt_nsec() const {
  return static_cast<double>(dt_);
}

double Ticker::total_sec() const {
  return static_cast<double>((last_ - start_) / 1e9);
}

double Ticker::total_usec() const {
  return static_cast<double>((last_ - start_) / 1e6);
}

double Ticker::total_msec() const {
  return static_cast<double>((last_ - start_) / 1e3);
}

double Ticker::total_nsec() const {
  return static_cast<double>(last_ - start_);
}

} // namespace hades
