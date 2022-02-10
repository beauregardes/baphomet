#pragma once

#include <chrono>

namespace baphomet {

using Clock = std::chrono::steady_clock;

using Duration = std::chrono::duration<double>;

inline double duration_to_double_secs(Duration d) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count() / 1e9;
}

} // namespace baphomet