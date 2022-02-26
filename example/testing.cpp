#include "testing.hpp"

class Testing : public baphomet::Application {
  float x_radius{250.0f}, y_radius{250.0f};

  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();
  }

  void draw() override {
    gfx->clear();

    if (ImGui::Begin("Oval")) {
      ImGui::DragFloat("x radius", &x_radius);
      ImGui::DragFloat("y radius", &y_radius);

      ImGui::End();
    }

    gfx->draw_oval(
        window->w() / 2.0f, window->h() / 2.0f,
        x_radius, y_radius,
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
