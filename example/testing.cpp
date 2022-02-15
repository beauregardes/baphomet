#include "testing.hpp"

class Testing : public baphomet::Application {
  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();
  }

  void draw() override {
    gfx->clear();

    gfx->circle(input->mouse.x, input->mouse.y, 10, baphomet::rgb(0xffffff));
  }
};

int main(int, char *[]) {
  baphomet::Runner(spdlog::level::debug)
      .open<Testing>({
          .title = "Testing",
          .size = {800, 600},
          .flags = baphomet::WFlags::centered
      })
      .init_gl()
      .start();
}

