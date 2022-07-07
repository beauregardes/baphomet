#pragma once

#include "pcg_random.hpp"
#include <cstdint>
#include <concepts>
#include <random>
#include <string>

namespace rnd {

struct seed_data {
  pcg_extras::pcg128_t seed{0};
  pcg_extras::pcg128_t stream{0};
};

pcg64 &generator();
seed_data &seed_info();

void reseed();

void seed128(
    std::uint64_t seed_hi, std::uint64_t seed_lo,
    std::uint64_t stream_hi = 0, std::uint64_t stream_lo = 0
);

void seed(std::uint64_t seed, std::uint64_t stream = 0);

void debug_show_seed();

/******************
 * HELPER CONCEPTS
 */

template<typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

/***********
 * INTEGERS
 */

template<typename T>
using IntDist = std::uniform_int_distribution<T>;

template<typename T>
concept IntDistCompatible =
    IsAnyOf<T, short, int, long, long long, unsigned short, unsigned int, unsigned long, unsigned long long>;

template<IntDistCompatible T>
T get(T low, T high) {
  if (low > high)
    return IntDist<T>(high, low)(generator());
  return IntDist<T>(low, high)(generator());
}

template<IntDistCompatible T>
T get(T high) {
  return IntDist<T>(T(0), high)(generator());
}

template<IntDistCompatible T>
T get() {
  return IntDist<T>(
      std::numeric_limits<T>::min(),
      std::numeric_limits<T>::max()
  )(generator());
}

/********
 * CHARS
 */

template<typename T>
concept IntDistCompatibleButChar = IsAnyOf<T, char, char8_t, char16_t, char32_t, wchar_t, unsigned char>;

template<IntDistCompatibleButChar T>
T get(T low, T high) {
  return static_cast<T>(get<int>(
      static_cast<int>(low),
      static_cast<int>(high)
  ));
}

template<IntDistCompatibleButChar T>
T get(T high) {
  return static_cast<T>(get<int>(static_cast<int>(high)));
}

template<IntDistCompatibleButChar T>
T get() {
  return static_cast<T>(get<int>(
      static_cast<int>(std::numeric_limits<T>::min()),
      static_cast<int>(std::numeric_limits<T>::max())
  ));
}

/********
 * REALS
 */

template<typename T>
using RealDist = std::uniform_real_distribution<T>;

template<typename T>
concept RealDistCompatible = IsAnyOf<T, float, double, long double>;

template<RealDistCompatible T>
T get(T low, T high) {
  if (low > high)
    return RealDist<T>(high, low)(generator());
  return RealDist<T>(low, high)(generator());
}

template<RealDistCompatible T>
T get(T high) {
  return RealDist<T>(T(0.0), high)(generator());
}

template<RealDistCompatible T>
T get(bool between_min_and_max = false) {
  if (between_min_and_max)
    return RealDist<T>(
        std::numeric_limits<T>::min(),
        std::numeric_limits<T>::max()
    )(generator());
  return RealDist<T>(T(0.0), T(1.0))(generator());
}

/*******
 * BOOL
 */

using BoolDist = std::bernoulli_distribution;

template<typename T>
concept BoolDistCompatible = IsAnyOf<T, bool>;

template<BoolDistCompatible T>
T get(double chance) {
  return BoolDist(chance)(generator());
}

/*******
 * MISC
 */

std::string base58(std::size_t length);

}