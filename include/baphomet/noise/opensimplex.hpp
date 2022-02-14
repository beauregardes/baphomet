#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace baphomet {

class opensimplex {
public:
  ////////////
  // SMOOTH //
  ////////////

  static double noise(std::int64_t seed, double x, double y);
  static double noise_improve_x(std::int64_t seed, double x, double y);

  static double noise_improve_xy(std::int64_t seed, double x, double y, double z);
  static double noise_improve_xz(std::int64_t seed, double x, double y, double z);
  static double noise_fallback(std::int64_t seed, double x, double y, double z);

  static double noise_improve_xyz_improve_xy(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xyz_improve_xz(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xyz(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xy_improve_zw(std::int64_t seed, double x, double y, double z, double w);
  static double noise_fallback(std::int64_t seed, double x, double y, double z, double w);

  static double octave(std::int64_t seed, double x, double y, int octaves, double persistence);
  static double octave_improve_x(std::int64_t seed, double x, double y, int octaves, double persistence);

  static double octave_improve_xy(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave_improve_xz(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave_fallback(std::int64_t seed, double x, double y, double z, int octaves, double persistence);

  static double octave_improve_xyz_improve_xy(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xyz_improve_xz(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xyz(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xy_improve_zw(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_fallback(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);

private:
  static constexpr std::int64_t PRIME_X_{0x5205402B9270C86FL};
  static constexpr std::int64_t PRIME_Y_{0x598CD327003817B5L};
  static constexpr std::int64_t PRIME_Z_{0x5BCC226E9FA0BACBL};
  static constexpr std::int64_t PRIME_W_{0x56CC5227E58F554BL};
  static constexpr std::int64_t HASH_MULTIPLIER_{0x53A3F72DEEC546F5L};
  static constexpr std::int64_t SEED_FLIP_3D_{-0x52D547B2E96ED629L};

  static constexpr double ROOT2OVER2_{0.7071067811865476};
  static constexpr double SKEW_2D_{0.366025403784439};
  static constexpr double UNSKEW_2D_{-0.21132486540518713};

  static constexpr double ROOT3OVER3_{0.577350269189626};
  static constexpr double FALLBACK_ROTATE3_{2.0 / 3.0};
  static constexpr double ROTATE3_ORTHOGONALIZER_ = UNSKEW_2D_;

  static constexpr double SKEW_4D_{0.309016994374947};
  static constexpr double UNSKEW_4D_{-0.138196601125011};

  static constexpr int N_GRADS_2D_EXPONENT_{7};
  static constexpr int N_GRADS_3D_EXPONENT_{8};
  static constexpr int N_GRADS_4D_EXPONENT_{9};
  static constexpr int N_GRADS_2D_{1 << N_GRADS_2D_EXPONENT_};
  static constexpr int N_GRADS_3D_{1 << N_GRADS_3D_EXPONENT_};
  static constexpr int N_GRADS_4D_{1 << N_GRADS_4D_EXPONENT_};

  static constexpr double NORMALIZER_2D_{0.05481866495625118};
  static constexpr double NORMALIZER_3D_{0.2781926117527186};
  static constexpr double NORMALIZER_4D_{0.11127401889945551};

  static constexpr double RSQUARED_2D_{2.0 / 3.0};
  static constexpr double RSQUARED_3D_{3.0 / 4.0};
  static constexpr double RSQUARED_4D_{4.0 / 5.0};

  static double noise_unskewed_base_(std::int64_t seed, double xs, double ys);
  static double noise_unrotated_base_(std::int64_t seed, double xr, double yr, double zr);
  static double noise_unskewed_base_(std::int64_t seed, double xs, double ys, double zs, double ws);

  static double grad_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp,
      double dx, double dy
  );

  static double grad_(
      std::int64_t seed,
      std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
      double dx, double dy, double dz
  );

  static double grad_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
      double dx, double dy, double dz, double dw
  );

  static const std::array<double, N_GRADS_2D_ * 2> GRADIENTS_2D_;
  static const std::array<double, N_GRADS_3D_ * 4> GRADIENTS_3D_;
  static const std::array<double, N_GRADS_4D_ * 4> GRADIENTS_4D_;

  struct LatticeVertex4D_ {
    double dx, dy, dz, dw;
    std::int64_t xsvp, ysvp, zsvp, wsvp;

    LatticeVertex4D_() = default;

    LatticeVertex4D_(std::int64_t xsv, std::int64_t ysv, std::int64_t zsv, std::int64_t wsv)
        : dx(-xsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dy(-ysv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dz(-zsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dw(-wsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          xsvp(xsv * PRIME_X_),
          ysvp(ysv * PRIME_Y_),
          zsvp(zsv * PRIME_Z_),
          wsvp(wsv * PRIME_W_) {}
  };

  static const std::array<int, 256> LOOKUP_4D_A_;
  static const std::array<LatticeVertex4D_, 3476> LOOKUP_4D_B_;

public:
  //////////
  // FAST //
  //////////

  static double noise_fast(std::int64_t seed, double x, double y);
  static double noise_improve_x_fast(std::int64_t seed, double x, double y);

  static double noise_improve_xy_fast(std::int64_t seed, double x, double y, double z);
  static double noise_improve_xz_fast(std::int64_t seed, double x, double y, double z);
  static double noise_fallback_fast(std::int64_t seed, double x, double y, double z);

  static double noise_improve_xyz_improve_xy_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xyz_improve_xz_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xyz_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise_improve_xy_improve_zw_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise_fallback_fast(std::int64_t seed, double x, double y, double z, double w);

  static double octave_fast(std::int64_t seed, double x, double y, int octaves, double persistence);
  static double octave_improve_x_fast(std::int64_t seed, double x, double y, int octaves, double persistence);

  static double octave_improve_xy_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave_improve_xz_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave_fallback_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);

  static double octave_improve_xyz_improve_xy_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xyz_improve_xz_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xyz_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_improve_xy_improve_zw_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave_fallback_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);

private:
  static constexpr std::int64_t SEED_OFFSET_4D_FAST_{0xE83DC3E0DA7164DL};

  static constexpr double SKEW_4D_FAST_{-0.138196601125011};
  static constexpr double UNSKEW_4D_FAST_{0.309016994374947};
  static constexpr double LATTICE_STEP_4D_FAST_{0.2};

  static constexpr double NORMALIZER_2D_FAST_{0.01001634121365712};
  static constexpr double NORMALIZER_3D_FAST_{0.07969837668935331};
  static constexpr double NORMALIZER_4D_FAST_{0.0220065933241897};

  static constexpr double RSQUARED_2D_FAST_{0.5};
  static constexpr double RSQUARED_3D_FAST_{0.6};
  static constexpr double RSQUARED_4D_FAST_{0.6};

  static double noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys);
  static double noise_unrotated_base_fast_(std::int64_t seed, double xr, double yr, double zr);
  static double noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys, double zs, double ws);

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp,
      double dx, double dy
  );

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
      double dx, double dy, double dz
  );

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
      double dx, double dy, double dz, double dw
  );

  static const std::array<double, N_GRADS_2D_ * 2> GRADIENTS_2D_FAST_;
  static const std::array<double, N_GRADS_3D_ * 4> GRADIENTS_3D_FAST_;
  static const std::array<double, N_GRADS_4D_ * 4> GRADIENTS_4D_FAST_;

private:
  /////////////
  // HELPERS //
  /////////////

  static int fast_floor_(double x);
  static int fast_round_(double x);

  static const std::array<double, 48> GRAD2_;
  static const std::array<double, 192> GRAD3_;
  static const std::array<double, 640> GRAD4_;

  static const std::array<std::vector<int>, 256> LOOKUP_4D_VERTEX_CODES_;
};

} // namespace baphomet