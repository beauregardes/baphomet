#include "hades/util/random.hpp"

#include "spdlog/spdlog.h"

namespace rnd {

void gen_seed_vals() {
  pcg_extras::pcg128_t seed_vals[4];
  auto seed_seq = pcg_extras::seed_seq_from<std::random_device>();
  pcg_extras::generate_to<2>(seed_seq, seed_vals);

  seed_info().seed = seed_vals[0];
  seed_info().stream = seed_vals[1];
}

pcg64 &generator() {
  static thread_local pcg64 g = std::invoke([] {
    gen_seed_vals();
    return pcg64(seed_info().seed, seed_info().stream);
  });

  return g;
}

seed_data &seed_info() {
  static thread_local seed_data s{};
  return s;
}

void reseed() {
  generator();  // Make sure the generator has been invoked before
  gen_seed_vals();
  generator().seed(seed_info().seed, seed_info().stream);
}

void seed128(
  std::uint64_t seed_hi, std::uint64_t seed_lo,
  std::uint64_t stream_hi, std::uint64_t stream_lo
) {
  pcg_extras::pcg128_t seed = PCG_128BIT_CONSTANT(seed_hi, seed_lo);
  pcg_extras::pcg128_t stream = PCG_128BIT_CONSTANT(stream_hi, stream_lo);
  generator().seed(seed, stream);
  seed_info().seed = seed;
  seed_info().stream = stream;
}

void seed(std::uint64_t seed, std::uint64_t stream) {
  generator().seed(seed, stream);
  seed_info().seed = seed;
  seed_info().stream = stream;
}

void debug_show_seed() {
  if (std::uint64_t(seed_info().seed >> 64) == 0 && std::uint64_t(seed_info().stream >> 64) == 0) {
    spdlog::debug(
      "Seed statement:\n"
      "  rnd::seed({:#x}, {:#x});",
      std::uint64_t(seed_info().seed),
      std::uint64_t(seed_info().stream)
    );
  } else {
    auto seed_hi = std::uint64_t(seed_info().seed >> 64);
    auto seed_lo = std::uint64_t(seed_info().seed);
    auto stream_hi = std::uint64_t(seed_info().stream >> 64);
    auto stream_lo = std::uint64_t(seed_info().stream);
    spdlog::debug(
      "Seed statement:\n"
      "  rnd::seed128(\n"
      "  {:#x}, {:#x},\n"
      "  {:#x}, {:#x}\n"
      "  );",
      seed_hi, seed_lo,
      stream_hi, stream_lo
    );
  }
}

hades::RGB rgb() {
  return hades::rgb(
    get<int>(0, 255),
    get<int>(0, 255),
    get<int>(0, 255)
  );
}

hades::RGB rgb(glm::ivec2 r_range, glm::ivec2 g_range, glm::ivec2 b_range) {
  return hades::rgb(
    get<int>(r_range.x, r_range.y),
    get<int>(g_range.x, g_range.y),
    get<int>(b_range.x, b_range.y)
  );
}

hades::RGB rgba() {
  return hades::rgba(
    get<int>(0, 255),
    get<int>(0, 255),
    get<int>(0, 255),
    get<int>(0, 255)
  );
}

hades::RGB rgba(glm::ivec2 r_range, glm::ivec2 g_range, glm::ivec2 b_range, glm::ivec2 a_range) {
  return hades::rgba(
    get<int>(r_range.x, r_range.y),
    get<int>(g_range.x, g_range.y),
    get<int>(b_range.x, b_range.y),
    get<int>(a_range.x, a_range.y)
  );
}

std::string base58(std::size_t length) {
  const static char B58_ALPHABET[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

  std::string id;
  id.reserve(length);

  for (int i = 0; i < length; i++)
    id += B58_ALPHABET[get<unsigned char>(0x00, 0xff) % 58];

  return id;
}

} // namespace rnd
