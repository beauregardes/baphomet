#include "testing.hpp"

class Testing : public baphomet::Application {
  std::shared_ptr<baphomet::RenderTarget> left{nullptr};
  std::shared_ptr<baphomet::RenderTarget> right{nullptr};

  std::unique_ptr<baphomet::Texture> test_tex{nullptr};

  void initialize() override {
    set_overlay_enabled(true);

    left = gfx->make_render_target(0, 0, 150, 200);
    right = gfx->make_render_target(150, 0, 150, 200);

    test_tex = gfx->load_texture("example/resources/test_tex.png", true);
  }

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear(baphomet::rgba(0x000000ff));

    gfx->push_render_target(left);
    gfx->clear(baphomet::rgba(0x00000000));
    test_tex->draw(59, 84);

    gfx->push_render_target(right);
    gfx->clear(baphomet::rgba(0x00000000));
    test_tex->draw(59, 84);
  }
};

BAPHOMET_GL_MAIN_DEBUG(
    Testing,
    .title = "Testing",
    .size = {300, 200},
    .flags = baphomet::WFlags::centered
)

