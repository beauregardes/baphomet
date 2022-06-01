#include "goat/util/time.hpp"
#include <chrono>

namespace goat {

double time_nsec() {
  using namespace std::chrono;

  auto now = steady_clock::now();
  return duration_cast<nanoseconds>(now.time_since_epoch()).count();
}

double time_usec() {
  return time_nsec() / 1e3;
}

double time_msec() {
  return time_nsec() / 1e6;
}

double time_sec() {
  return time_nsec() / 1e9;
}

} // namespace goat
