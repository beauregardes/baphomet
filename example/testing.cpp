#include "testing.hpp"

class Testing : public baphomet::Application {
  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();
  }

  void draw() override {
    gfx->clear();
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

