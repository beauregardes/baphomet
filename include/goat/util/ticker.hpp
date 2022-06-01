#pragma once

#include <cstdint>

namespace goat {

class Ticker {
public:
  Ticker(double interval = 0.0);

  std::uint64_t tick();

  double dt_nsec();
  double dt_usec();
  double dt_msec();
  double dt_sec();

private:
  double start_{};
  double last_{};
  double dt_{0.0};
  double interval_{};
  double acc_{0.0};
};

} // namespace goat
