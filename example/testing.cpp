#include "testing.hpp"

std::string hello() {
  return "hello";
}

class Testing : public baphomet::Application {
  std::unique_ptr<baphomet::Texture> p_tex{nullptr};
  std::shared_ptr<baphomet::ParticleSystem> ps{nullptr};

  void initialize() override {
    set_overlay_enabled(true);

    p_tex = gfx->load_texture("example/resources/spiral.png", true);
    ps = gfx->make_particle_system(p_tex);
    ps->set_emitter_rate(25000);
//    ps->set_particle_limit(25000);
    ps->set_ttl(0.5s, 1.5s);
    ps->set_spread(0, 360);
    ps->set_radial_vel(50, 100);
    ps->set_radial_accel(-30, 30);
    ps->set_spin(-720, 720);
    ps->set_colors({
      baphomet::rgba(0xffffff80),
      baphomet::rgba(0x80ffff40),
      baphomet::rgba(0x8080ff00)
    });

    sticky("Live particles", [&]{ return ps->live_count(); });
    sticky("Hello", hello);
  }

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape"))
      shutdown();

    if (input->down("mb_left"))
      ps->emit(dt, input->mouse.x, input->mouse.y);

    if (input->pressed("mb_right"))
      ps->emit_count(50, input->mouse.x, input->mouse.y);
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
    .flags = baphomet::WFlags::centered | baphomet::WFlags::vsync
)
