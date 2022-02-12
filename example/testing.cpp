#include "testing.hpp"

class Testing : public baphomet::Application {
protected:
  baphomet::Rect r{50, 50, 50, 50};
  double angle = 0;

  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();

    if (input->pressed("mb_left")) {
      tween->tween("x", r.x, r.x, input->mouse.x - r.w / 2, 2s, baphomet::Easing::in_out_bounce);
      tween->tween("y", r.y, r.y, input->mouse.y - r.h / 2, 2s, baphomet::Easing::in_out_bounce);
    }

    angle += 180 * (dt / 1s);
    if (angle > 360.0)
      angle -= 360.0;
  }

  void draw() override {
    gfx->clear();

    gfx->rect(r, baphomet::rgb(0xff0000), angle);
  }
};

int main(int, char *[]) {
  baphomet::Runner(spdlog::level::debug)
      .open<Testing>({
          .title = "Testing",
          .size = {800, 600},
          .flags = baphomet::WFlags::centered | baphomet::WFlags::vsync
      })
      .init_gl()
      .start();
}

