#include "testing.hpp"

class Testing : public baphomet::Application {
  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();
  }

  void draw() override {
    gfx->clear();

    gfx->draw_oval(
        window->w() / 2.0f, window->h() / 2.0f,
        100.0f, 100.0f,
        baphomet::rgb(0xffffff)
    );
  }
};

BAPHOMET_GL_MAIN_DEBUG(
    Testing,
    .title = "Testing",
    .size = {800, 600},
    .flags = baphomet::WFlags::centered
)
