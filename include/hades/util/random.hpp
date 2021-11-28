#ifndef HADES_UTIL_RANDOM_HPP
#define HADES_UTIL_RANDOM_HPP

#include "glm/glm.hpp"
#include "pcg_random.hpp"

#include "hades/color.hpp"

#include <concepts>
#include <iterator>
#include <limits>
#include <random>
#include <ranges>
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
IsAnyOf<T, short, int, long, long long,
  unsigned short, unsigned int, unsigned long, unsigned long long>;

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
    std::numeric_limits<T>::max())(generator());
}

/********
 * CHARS
 */

template<typename T>
concept IntDistCompatibleButChar =
IsAnyOf<T, char, char8_t, char16_t, char32_t, wchar_t, unsigned char>;

template<IntDistCompatibleButChar T>
T get(T low, T high) {
  return static_cast<T>(get<int>(
    static_cast<int>(low),
    static_cast<int>(high)));
}

template<IntDistCompatibleButChar T>
T get(T high) {
  return static_cast<T>(get<int>(static_cast<int>(high)));
}

template<IntDistCompatibleButChar T>
T get() {
  return static_cast<T>(get<int>(
    static_cast<int>(std::numeric_limits<T>::min()),
    static_cast<int>(std::numeric_limits<T>::max())));
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
      std::numeric_limits<T>::max())(generator());
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

/*******************
 * RANGES/ITERATORS
 */

template<std::input_iterator T>
T choose(T begin, T end) {
  auto d = std::distance(begin, end);
  return std::next(begin, get(d - 1));
}

template<std::ranges::sized_range T>
std::iter_value_t<T> choose(const T &range) {
  auto size = std::ranges::size(range);
  return *std::next(std::ranges::begin(range), get(size - 1));
}

template<typename T>
T choose(const std::initializer_list<T> list) {
  auto size = std::ranges::size(list);
  return *std::next(std::ranges::begin(list), get(size - 1));
}

template<std::size_t N, typename T>
T choose(const T (&arr)[N]) {
  return arr[get(N - 1)];
}

template<typename T>
T choose(const T *arr, std::size_t length) {
  return arr[get(length - 1)];
}

/**********
 * STRINGS
 */

template<typename T>
concept StringCompatible =
IsAnyOf<T, std::string, std::u8string, std::u16string, std::u32string, std::wstring>;

template<
  StringCompatible T,
  std::size_t N,
  typename CharT = typename T::value_type>
T get(CharT low, CharT high) {
  T s {};
  s.reserve(N);
  for (std::size_t i = 0; i < N; ++i)
    s += get(low, high);
  return s;
}

template<
  StringCompatible T,
  std::size_t N,
  std::ranges::sized_range Options,
  typename CharT = typename T::value_type>
T get(const Options &options) {
  T s {};
  s.reserve(N);
  for (std::size_t i = 0; i < N; ++i)
    s += static_cast<CharT>(choose(options));
  return s;
}

template<
  StringCompatible T,
  std::size_t N,
  typename OptionsT,
  typename CharT = typename T::value_type>
T get(const std::initializer_list<OptionsT> options) {
  T s {};
  s.reserve(N);
  for (std::size_t i = 0; i < N; ++i)
    s += static_cast<CharT>(choose(options));
  return s;
}

template<
  StringCompatible T,
  std::size_t N,
  typename CharT = typename T::value_type>
T get() {
  T s {};
  s.reserve(N);
  for (std::size_t i = 0; i < N; ++i)
    s += get<CharT>();
  return s;
}

/*********
 * COLORS
 */

hades::RGB rgb();
hades::RGB rgb(glm::ivec2 r_range, glm::ivec2 g_range, glm::ivec2 b_range);

hades::RGB rgba();
hades::RGB rgba(glm::ivec2 r_range, glm::ivec2 g_range, glm::ivec2 b_range, glm::ivec2 a_range);

/*******
 * MISC
 */

std::string base58(std::size_t length);

} // namespace rnd

#endif //HADES_UTIL_RANDOM_HPP
