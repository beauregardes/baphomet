#include "testing.hpp"

class Testing : public baphomet::Application {
  std::shared_ptr<baphomet::RenderTarget> left{nullptr};
  std::shared_ptr<baphomet::RenderTarget> right{nullptr};

  void initialize() override {
    set_overlay_enabled(true);

    left = gfx->make_render_target(0, 0, 400, 600);
    right = gfx->make_render_target(400, 0, 400, 600);
  }

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(baphomet::rgba(0x101010ff));
    gfx->clear();
    gfx->fill_circle(400, 300, 50, baphomet::rgb(0xff8080));

    gfx->push_render_target(left);
    gfx->clear_color(baphomet::rgba(0x00000000));
    gfx->clear();
    gfx->fill_circle(200, 300, 25, baphomet::rgb(0x80ff80));
    gfx->pop_render_target();

    gfx->push_render_target(right);
    gfx->clear_color(baphomet::rgba(0x00000000));
    gfx->clear();
    gfx->fill_circle(200, 300, 25, baphomet::rgb(0x8080ff));
    gfx->pop_render_target();
  }
};

BAPHOMET_GL_MAIN_DEBUG(
    Testing,
    .title = "Testing",
    .size = {800, 600},
    .flags = baphomet::WFlags::centered
)
