#include "testing.hpp"

class Testing : public baphomet::Application {
protected:
  double x = 50;
  double y = 50;

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape")) shutdown();

    if (input->down("a", 500ms, 1s))
      debug_log("Pressing a!");

    if (input->pressed("1")) {
      tween->tween("x", x, x, 200.0, 1s, baphomet::Easing::in_cubic);
      tween->tween("y", y, y, 200.0, 1s, baphomet::Easing::out_cubic);
    }

    if (input->pressed("2")) {
      tween->toggle("x");
      tween->toggle("y");
    }
  }

  void draw() override {
    gfx->clear();

    gfx->rect(x, y, 50, 50, baphomet::rgb(0xff0000));
  }
};

int main(int, char *[]) {
  baphomet::Runner()
      .open<Testing>({
          .title = "Testing",
          .size = {800, 600},
          .flags = baphomet::WFlags::centered
      })
      .init_gl()
      .start();
}