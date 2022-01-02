#include "hades/hades.hpp"

#include <vector>

class Testing : public hades::Application {
public:
  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();

//    for (int w = 0; w < window->h(); w++)
//      draw_line(0, w, w, w);

    draw_line(0, 0, 0, 3, hades::rgb(0xff8080));
  }

  void draw_line(float x0, float y0, float x1, float y1, const hades::RGB &color) {
    const static std::vector<glm::vec3> lt90_line_width_ranges = {
        {0.802851f, 0.767945f, 0.7199f}, // 46-44
        {0.837758f, 0.733038f, 0.743f},  // 48-42
        {0.959931f, 0.610865f, 0.81f},   // 55-35
        {1.0472f,   0.523599f, 0.87f},   // 60-30
        {1.13446f,  0.436332f, 0.9f},    // 65-25
        {1.309f,    0.261799f, 0.965f}   // 75-15
    };

    const static std::vector<glm::vec3> gt90_line_width_ranges = {
        {2.37365f, 2.33874f, 0.7199f},  // 136-134
        {2.40855f, 2.30383f, 0.743f},   // 138-132
        {2.53073f, 2.18166f, 0.81f},    // 145-125
        {2.61799f, 2.0944f,  0.87f},    // 150-120
        {2.70526f, 2.00713f, 0.9f},     // 155-115
        {2.87979f, 1.8326f,  0.965f}    // 165-105
    };

    float dx = x1 - x0;
    float dy = y1 - y0;
    float mag = std::sqrt(dx * dx + dy * dy);
    float norm_dy = dy / mag;
    float norm_dx = dx / mag;
    float angle = std::abs(std::atan2(norm_dy, norm_dx));

    float width = 1.0f;
    for (const auto &r: angle < 1.570796f ? lt90_line_width_ranges : gt90_line_width_ranges)
      if (r[0] > angle && angle > r[1]) {
        width = r[2];
        break;
      }

    float half_w = width / 2.0f;
    float scaled_dx = -norm_dy * half_w;
    float scaled_dy = norm_dx * half_w;

    gfx->tri(x0,             y0,             x0 + scaled_dx, y0 + scaled_dy, x1, y1, color);
    gfx->tri(x0 + scaled_dx, y0 + scaled_dy, x1 + scaled_dx, y1 + scaled_dy, x1, y1, color);
    gfx->tri(x0,             y0,             x0 - scaled_dx, y0 - scaled_dy, x1, y1, color);
    gfx->tri(x0 - scaled_dx, y0 - scaled_dy, x1 - scaled_dx, y1 - scaled_dy, x1, y1, color);
  }
};

int main(int, char *[]) {
  hades::Runner()
      .open<Testing>({
          .title = "Testing",
          .size = {800, 800},
          .monitor = 1,
          .flags = hades::WFlags::centered
      })
      .initgl()
      .start();
}
