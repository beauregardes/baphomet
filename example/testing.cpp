#include "testing.hpp"

class Testing : public baphomet::Application {
  std::unique_ptr<baphomet::Texture> p_tex{nullptr};
  std::shared_ptr<baphomet::ParticleSystem> ps{nullptr};

  void initialize() override {
    set_overlay_enabled(true);

    p_tex = gfx->load_texture("example/resources/spiral.png", true);
    ps = gfx->make_particle_system(p_tex);
    ps->set_emitter_rate(20000);
//    stars->set_particle_limit(50000);
    ps->set_ttl(1s, 2s);
    ps->set_spread(0, 360);
    ps->set_radial_vel(50, 50);
    ps->set_radial_accel(10, 50);
    ps->set_spin(90, 360);
    ps->set_colors({
      baphomet::rgba(0xffffffff),
      baphomet::rgba(0x80ffffff),
      baphomet::rgba(0x8080ff00)
    });

    timer->every(0.25s, [&]{
      debug_log("Live particles: {}", ps->live_count());
    });
  }

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape"))
      shutdown();

    if (input->down("mb_left"))
      ps->emit(dt, input->mouse.x, input->mouse.y);
  }

  void draw() override {
    gfx->clear();

    ps->draw();

    gfx->fill_rect(100, 100, 30, 30, baphomet::rgba(0xfffffff));
    gfx->fill_rect(140, 100, 30, 30, baphomet::rgba(0xfffffcc));
    gfx->fill_rect(180, 100, 30, 30, baphomet::rgba(0xfffff99));
    gfx->fill_rect(220, 100, 30, 30, baphomet::rgba(0xfffff66));
    gfx->fill_rect(260, 100, 30, 30, baphomet::rgba(0xfffff33));
  }
};

BAPHOMET_GL_MAIN_DEBUG(
    Testing,
    .title = "Testing",
    .size = {800, 600},
    .flags = baphomet::WFlags::centered
)
