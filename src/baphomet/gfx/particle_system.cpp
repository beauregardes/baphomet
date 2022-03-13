#include "baphomet/gfx/particle_system.hpp"

#include "glm/glm.hpp"

using namespace std::chrono_literals;

namespace baphomet {

ParticleSystem::ParticleSystem(std::unique_ptr<baphomet::Texture> &tex)
    : tex_(tex) {}

std::size_t ParticleSystem::live_count() {
  return live_count_;
}

void ParticleSystem::set_emitter_pos(float x, float y) {
  params_.emitter_pos = {x, y};
}

void ParticleSystem::set_emitter_rate(float particles_per_second) {
  params_.emitter_rate = particles_per_second;
}

void ParticleSystem::set_particle_limit(std::size_t particle_limit) {
  params_.particle_limit = particle_limit;
  particles_.reserve(particle_limit);
}

void ParticleSystem::set_ttl(baphomet::Duration min, baphomet::Duration max) {
  params_.ttl_min = min;
  params_.ttl_max = max;
}

void ParticleSystem::set_spread(float min, float max) {
  params_.angle_min = glm::radians(min);
  params_.angle_max = glm::radians(max);
}

void ParticleSystem::set_radial_vel(float min, float max) {
  params_.delta_min = min;
  params_.delta_max = max;
}

void ParticleSystem::set_radial_accel(float min, float max) {
  params_.accel_min = min;
  params_.accel_max = max;
}

void ParticleSystem::set_spin(float min, float max) {
  params_.spin_min = min;
  params_.spin_max = max;
}

void ParticleSystem::set_colors(const std::vector<baphomet::RGB> &colors) {
  params_.colors.clear();

  float step = 1.0f / (colors.size() - 1);
  float acc = 0.0;
  for (const auto &color : colors) {
    params_.colors.emplace_back(acc, color);
    acc += step;
  }
}

void ParticleSystem::emit_count(std::size_t count, float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return;

  std::size_t search_start = 0;
  for (std::size_t i = 0; i < count && live_count_ < params_.particle_limit; ++i)
    search_start = find_insert_particle_(search_start, x, y);
}

void ParticleSystem::emit_count(std::size_t count) {
  emit_count(count, params_.emitter_pos.x, params_.emitter_pos.y);
}

void ParticleSystem::emit(Duration dt, float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return;

  std::size_t search_start = 0;
  auto acc_step = baphomet::sec(1 / params_.emitter_rate);

  params_.emitter_acc += dt;
  while (params_.emitter_acc >= acc_step && live_count_ < params_.particle_limit) {
    search_start = find_insert_particle_(search_start, x, y);
    params_.emitter_acc -= acc_step;
  }

  while (params_.emitter_acc >= acc_step) {
    params_.emitter_acc -= acc_step;
  }
}

void ParticleSystem::emit(Duration dt) {
  emit(dt, params_.emitter_pos.x, params_.emitter_pos.y);
}

void ParticleSystem::draw() {
  for (const auto &p : particles_) {
    if (!p.alive)
      continue;

    tex_->draw(
        p.x - (p.w / 2), p.y - (p.h / 2),
        p.w, p.h,
        p.x, p.y, p.tex_angle,
        p.color
    );
  }
}

float lerp(float a, float b, float t) {
  return a + t * (b - a);
}

void ParticleSystem::update_(Duration dt) {
  for (auto &p : particles_) {
    if (!p.alive)
      continue;

    p.acc += dt;
    if (p.acc >= p.ttl) {
      p.alive = false;
      live_count_--;
      continue;
    }

    p.radial_vel += p.radial_accel * (dt / 1s);
    p.x += std::cos(p.angle) * p.radial_vel * (dt / 1s);
    p.y += -std::sin(p.angle) * p.radial_vel * (dt / 1s);

    p.tex_angle += p.spin * (dt / 1s);

    auto progress = p.acc / p.ttl;
    for (std::size_t i = 0; i < params_.colors.size() - 1; ++i) {
      auto [prog1, color1] = params_.colors[i];
      auto [prog2, color2] = params_.colors[i + 1];
      if (progress >= prog1 && progress <= prog2) {
        float numer = progress - prog1;
        float denom = prog2 - prog1;
        p.color = baphomet::rgba(
            lerp(color1.r, color2.r, numer / denom),
            lerp(color1.g, color2.g, numer / denom),
            lerp(color1.b, color2.b, numer / denom),
            lerp(color1.a, color2.a, numer / denom)
        );
        break;
      }
    }
  }
}

std::size_t ParticleSystem::find_insert_particle_(std::size_t search_start, float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return search_start;

  for (std::size_t i = search_start; i < particles_.size(); ++i) {
    if (!particles_[i].alive) {
      particles_[i].ttl = baphomet::sec(rnd::get<float>(params_.ttl_min / 1s, params_.ttl_max / 1s));
      particles_[i].angle = rnd::get<float>(params_.angle_min, params_.angle_max);
      particles_[i].radial_vel = rnd::get<float>(params_.delta_min, params_.delta_max);
      particles_[i].radial_accel = rnd::get<float>(params_.accel_min, params_.accel_max);
      particles_[i].spin = rnd::get<float>(params_.spin_min, params_.spin_max),
      particles_[i].x = x;
      particles_[i].y = y;
      particles_[i].w = static_cast<float>(tex_->w());
      particles_[i].h = static_cast<float>(tex_->h());
      particles_[i].tex_angle = 0.0f;
      particles_[i].color = std::get<1>(params_.colors[0]);
      particles_[i].acc = baphomet::sec(0);
      particles_[i].alive = true;
      live_count_++;

      return i + 1;
    }
  }

  particles_.emplace_back(
      baphomet::sec(rnd::get<float>(params_.ttl_min / 1s, params_.ttl_max / 1s)),
      rnd::get<float>(params_.angle_min, params_.angle_max),
      rnd::get<float>(params_.delta_min, params_.delta_max),
      rnd::get<float>(params_.accel_min, params_.accel_max),
      rnd::get<float>(params_.spin_min, params_.spin_max),
      x,
      y,
      static_cast<float>(tex_->w()),
      static_cast<float>(tex_->h()),
      0.0f,
      std::get<1>(params_.colors[0])
  );
  live_count_++;

  return particles_.size();
}

} // namespace baphomet