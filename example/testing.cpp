#include "testing.hpp"

#include <cmath>

const auto RESOURCES = fs::path(__FILE__).parent_path() / "resources";

class Testing : public hades::Application {
public:
  std::unique_ptr<hades::AudioMgr> audio{nullptr};
  std::vector<std::string> notes = {"c", "e", "g"};
  float volume = 1.0f;

  void initialize() override {
    audio = std::make_unique<hades::AudioMgr>();
    audio->open_device();
    audio->open_context();
    audio->make_current();

    for (const auto &n : notes)
      audio->load(n, (RESOURCES / "wav" / (n + ".wav")).string());

    timer->every({0.1}, [&]{
      auto note = rnd::choose(notes);
      audio->play(note, {.volume = volume});
      debug_log(note);
    });
  }

  void update(double dt) override {
    volume = std::abs(std::sin(glfwGetTime()));

    audio->update();

    if (input->pressed("escape"))
      shutdown();

    if (input->pressed("1"))
      audio->reopen_device();
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();

    gfx->oval(
        window->w() / 2,
        window->h() - (window->h() * volume),
        20, 10,
        hades::rgb(0xffffff),
        volume * 360
    );
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