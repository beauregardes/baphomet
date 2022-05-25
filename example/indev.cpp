#include "goat/goat.hpp"
#include "glbinding/gl/gl.h"

class Indev : public goat::Application {
public:
  void initialize() override {

  }

  void update(double dt) override {

  }

  void draw() override {
    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
  }
};

int main(int, char *[]) {
  goat::Engine().open<Indev>({
      .title = "Indev",
      .size = {800, 600},
      .monitor = 1,
      .flags = goat::WFlags::centered,
      .log_level = spdlog::level::debug
  });
}
