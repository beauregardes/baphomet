#include "testing.hpp"

const auto RESOURCES = fs::path(__FILE__).parent_path() / "resources";

class Testing : public hades::Application {
public:
  void initialize() override {
    audio->load("c", (RESOURCES / "wav" / "c_sine.wav").string());
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();

    if (input->pressed("1"))
      audio->play("c", {.looping = true});
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();
  }
};

int main(int, char *[]) {
  hades::Runner()
      .open<Testing>({
          .title = "Testing",
          .size = {400, 400},
          .monitor = 1,
          .flags = hades::WFlags::centered
      })
      .init_gl()
      .start();
}