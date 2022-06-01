#include "goat/goat.hpp"

class Indev : public goat::Application {
public:
  void initialize() override {

  }

  void update(double dt) override {
    if (input->pressed("1"))
      fmt::print("You pressed 1!\n");

    if (input->released("1"))
      fmt::print("You released 1!\n");

    if (input->pressed("mb_left"))
      fmt::print("Left click: {} {}\n", input->mouse.x, input->mouse.y);
  }

  void draw() override {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
};

int main(int, char *[]) {
  goat::Engine({
      .backend = goat::Backend::gl,
      .backend_version = {3, 3},
      .log_level = spdlog::level::debug
  }).open<Indev>({
      .title = "Indev",
      .size = {800, 600},
      .monitor = 1,
      .flags = goat::WFlags::centered
  });
}
