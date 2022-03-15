#include "testing.hpp"

class Testing : public baphomet::Application {
  std::unique_ptr<baphomet::Texture> p_tex{nullptr};
  std::shared_ptr<baphomet::ParticleSystem> ps{nullptr};

  void initialize() override {
    set_overlay_enabled(true);

    p_tex = gfx->load_texture("example/resources/spiral.png", true);
    ps = gfx->make_particle_system(p_tex);
    ps->set_emitter_pos(window->w() / 2, window->h() / 2);
    ps->set_emitter_rate(50000);
//    ps->set_particle_limit(50000);
    ps->set_ttl(0.5s, 1.5s);
    ps->set_spread(0, 180);
    ps->set_radial_vel(0, 50);
    ps->set_radial_accel(0, 50);
    ps->set_linear_accel(0, 0, 0, 250);
    ps->set_spin(-720, 720);
    ps->set_colors({
      baphomet::rgba(0xffffff80),
      baphomet::rgba(0xffffff80),
      baphomet::rgba(0xffffff80),
      baphomet::rgba(0x80ffff40),
      baphomet::rgba(0x8080ff00)
    });

    sticky("Live particles", [&]{ return ps->live_count(); });
  }

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape"))
      shutdown();

    ps->emit(dt);
  }

  void draw() override {
    gfx->clear();

    ps->draw();
  }
};

BAPHOMET_GL_MAIN_DEBUG(
    Testing,
    .title = "Testing",
    .size = {800, 600},
    .flags = baphomet::WFlags::centered
)
