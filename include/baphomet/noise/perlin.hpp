#pragma once

#include <array>

namespace baphomet {

class perlin {
public:
  static double noise(double x, double y, double z);
  static double noise(double x, double y);
  static double noise(double x);

  static double octave(double x, double y, double z, int octaves, double persistence);
  static double octave(double x, double y, int octaves, double persistence);
  static double octave(double x, int octaves, double persistence);

private:
  static double fade_(double t);

  static double lerp_(double t, double a, double b);

  static double grad_(int hash, double x, double y, double z);
  static double grad_(int hash, double x, double y);
  static double grad_(int hash, double x);

  static std::array<int, 512> p_;
};

} // namespace baphomet