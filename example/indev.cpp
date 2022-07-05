#include "goat/goat.hpp"

class Indev : public goat::Application {
public:
  void initialize() override {

  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
  }

  void draw() override {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ImGui::Begin("Info")) {
      ImGui::Text("Hello, world!");
    }
    ImGui::End();
  }
};

int main(int, char *[]) {
  goat::Engine({
//      .backend = goat::Backend::gl,
      .backend_version = {3, 3},
      .log_level = spdlog::level::debug
  }).open<Indev>({
      .title = "Indev",
      .size = {800, 600},
      .monitor = 0,
      .flags = goat::WFlags::centered
  });
}
