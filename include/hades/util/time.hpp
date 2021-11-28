#ifndef HADES_UTIL_TIME_HPP
#define HADES_UTIL_TIME_HPP

#include <chrono>
#include <concepts>
#include <cstdint>

namespace hades {

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T>
T time_nsec() {
  using namespace std::chrono;

  auto now = steady_clock::now().time_since_epoch();
  return static_cast<T>(duration_cast<nanoseconds>(now).count());
}

template<Numeric T>
T time_usec() {
  return static_cast<T>(time_nsec<std::uint64_t>() / std::uint64_t(1e3));
}

template<Numeric T>
T time_msec() {
  return static_cast<T>(time_nsec<std::uint64_t>() / std::uint64_t(1e6));
}

template<Numeric T>
T time_sec() {
  return static_cast<T>(time_nsec<std::uint64_t>() / std::uint64_t(1e9));
}

}

#endif //HADES_UTIL_TIME_HPP
