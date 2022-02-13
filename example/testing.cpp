#include "testing.hpp"

#include "baphomet/noise/perlin.hpp"

class Testing : public baphomet::Application {
  const double SCALE = 4.0;
  double z = 0.0;

  void initialize() override {}

  void update(baphomet::Duration dt) override {
    if (input->pressed("escape")) shutdown();

    z += 0.5 * (dt / 1s);
  }

  void draw() override {
    gfx->clear();

    for (int x = 0; x < window->w() / SCALE; ++x)
      for (int y = 0; y < window->h() / SCALE; ++y) {
        double v = baphomet::perlin::octave(
            x / (window->w() / SCALE) * 8,
            y / (window->h() / SCALE) * 6,
            z,
            3, 0.2
        );
        gfx->rect(
            x * SCALE, y * SCALE,
            SCALE, SCALE,
            baphomet::rgb(v * 255.0, v * 255.0, v * 255.0)
        );
      }

    double y0 = -1;
    for (int x = 0; x < window->w(); ++x) {
      double y1 = baphomet::perlin::octave(x / (double)window->w() * 8, z, 3, 0.4) * window->h();
      if (y0 != -1)
        gfx->line(x - 1, y0, x, y1, baphomet::rgb(0xff8080));
      y0 = y1;
    }

    y0 = -1;
    for (int x = 0; x < window->w(); ++x) {
      double y1 = baphomet::perlin::octave(x / (double)window->w() * 8, z + 91934.3, 3, 0.4) * window->h();
      if (y0 != -1)
        gfx->line(x - 1, y0, x, y1, baphomet::rgb(0x80ff80));
      y0 = y1;
    }

    y0 = -1;
    for (int x = 0; x < window->w(); ++x) {
      double y1 = baphomet::perlin::octave(x / (double)window->w() * 8, z + 9192392.2, 3, 0.4) * window->h();
      if (y0 != -1)
        gfx->line(x - 1, y0, x, y1, baphomet::rgb(0x8080ff));
      y0 = y1;
    }
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

