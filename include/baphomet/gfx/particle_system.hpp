#pragma once

#include "baphomet/gfx/texture.hpp"
#include "baphomet/util/time/time.hpp"
#include "baphomet/util/random.hpp"
#include "baphomet/util/shapes.hpp"

#include <vector>
#include <tuple>
#include <numbers>
#include <stack>

namespace baphomet {

struct Particle {
  baphomet::Duration ttl;
  float angle;
  float radial_vel;
  float radial_accel;

  float ldx, ldy;
  float lax, lay;

  float spin;

  float x, y;
  float w, h;
  float tex_angle;
  baphomet::RGB color;

  std::size_t color_idx{0};
  baphomet::Duration acc{baphomet::sec(0)};
  bool alive{true};

  Particle(
      baphomet::Duration ttl,
      float angle,
      float radial_vel,
      float radial_accel,
      float ldx, float ldy,
      float lax, float lay,
      float spin,
      float x, float y,
      float w, float h,
      float tex_angle = 0.0f,
      baphomet::RGB color = baphomet::rgb(0xffffff)
  ) : ttl(ttl),
      angle(angle),
      radial_vel(radial_vel),
      radial_accel(radial_accel),
      ldx(ldx), ldy(ldy),
      lax(lax), lay(lay),
      spin(spin),
      x(x), y(y),
      w(w), h(h),
      tex_angle(angle),
      color(color) {};
};

class ParticleSystem {
  friend class GfxMgr;

public:
  ParticleSystem(std::unique_ptr<baphomet::Texture> &tex);

  std::size_t live_count();

  void set_emitter_pos(float x, float y);
  void set_emitter_rate(float particles_per_second);

  void set_particle_limit(std::size_t particle_limit);

  void set_ttl(baphomet::Duration min, baphomet::Duration max);

  void set_spread(float min, float max);
  void set_radial_vel(float min, float max);
  void set_radial_accel(float min, float max);

  void set_linear_vel(float xmin, float xmax, float ymin, float ymax);
  void set_linear_accel(float xmin, float xmax, float ymin, float ymax);

  void set_spin(float min, float max);

  void set_colors(const std::vector<baphomet::RGB> &colors);

  void emit_count(std::size_t count, float x, float y);
  void emit_count(std::size_t count);

  void emit(Duration dt, float x, float y);
  void emit(Duration dt);

  void draw();

private:
  std::unique_ptr<baphomet::Texture> &tex_;

  TexRenderFunc render_func_;
  float tex_width_;
  float tex_height_;

  std::vector<Particle> particles_{};
  std::size_t live_count_{0};
  std::stack<std::size_t> dead_positions_{};

  struct {
    Point emitter_pos{0, 0};
    float emitter_rate{0};
    Duration emitter_acc{0};

    std::size_t particle_limit{std::numeric_limits<std::size_t>::max()};

    baphomet::Duration ttl_min{0}, ttl_max{0};
    float angle_min{0}, angle_max{2 * std::numbers::pi};
    float delta_min{0}, delta_max{0};
    float accel_min{0}, accel_max{0};

    float ldx_min{0}, ldx_max{0};
    float ldy_min{0}, ldy_max{0};
    float lax_min{0}, lax_max{0};
    float lay_min{0}, lay_max{0};

    float spin_min{0}, spin_max{0};

    std::vector<std::tuple<float, baphomet::RGB>> colors{
        {0.0f, baphomet::rgb(0xffffff)},
        {1.0f, baphomet::rgb(0xffffff)}
    };
  } params_;

  void update_(Duration dt);

  void find_insert_particle_(float x, float y);
};

} // namespace baphomet