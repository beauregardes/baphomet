#include "hades/util/shapes.hpp"

namespace hades {
namespace helpers {
namespace internal {

void plot_line_low(
    int x0, int y0, int x1, int y1,
    std::function<void(int x, int y)> f
) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int d = (2 * dy) - dx;
  int y = y0;

  for (int x = x0; x <= x1; x++) {
    f(x, y);
    if (d > 0) {
      y += yi;
      d += 2 * (dy - dx);
    } else
      d += 2 * dy;
  }
}

void plot_line_high(
    int x0, int y0, int x1, int y1,
    std::function<void(int x, int y)> f
) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int d = (2 * dx) - dy;
  int x = x0;

  for (int y = y0; y < y1; y++) {
    f(x, y);
    if (d > 0) {
      x += xi;
      d += 2 * (dx - dy);
    } else
      d += 2 * dx;
  }
}

} // namespace internal

double map(double n, double n_min, double n_max, double a, double b) {
  return (b - a) * ((n - n_min) / (n_max - n_min)) + a;
}

double lerp(double a, double b, double t) {
  return a + t * (b - a);
}

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p3, int steps,
    std::function<void(double x, double y)> f
) {
  double x, y;
  for (int t = 0; t < steps; t++) {
    double td = 1 / (double)(steps - 1);
    double itd = 1 - td;
    x = itd * itd * p0.x +
        2 * itd * td * p1.x +
        td * td * p3.x;
    y = itd * itd * p0.y +
        2 * itd * td * p1.y +
        td * td * p3.y;
    f(x, y);
  }
}

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p3, int steps,
    std::function<void(double x, double y, bool first_point)> f
) {
  bool first_point = true;
  double x, y;
  for (int t = 0; t < steps; t++) {
    double td = 1 / (double)(steps - 1);
    double itd = 1 - td;
    x = itd * itd * p0.x +
        2 * itd * td * p1.x +
        td * td * p3.x;
    y = itd * itd * p0.y +
        2 * itd * td * p1.y +
        td * td * p3.y;
    f(x, y, first_point);
    first_point = false;
  }
}

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int steps,
    std::function<void(double x, double y)> f
) {
  double x, y;
  for (int t = 0; t < steps; t++) {
    double td = t / (double)(steps - 1);
    double itd = 1 - td;
    x = itd * itd * itd * p0.x +
        3 * itd * itd * td * p1.x +
        3 * itd * td * td * p2.x +
        td * td * td * p3.x;
    y = itd * itd * itd * p0.y +
        3 * itd * itd * td * p1.y +
        3 * itd * td * td * p2.y +
        td * td * td * p3.y;
    f(x, y);
  }
}

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int steps,
    std::function<void(double x, double y, bool first_point)> f
) {
  bool first_point = true;
  double x, y;
  for (int t = 0; t < steps; t++) {
    double td = t / (double)(steps - 1);
    double itd = 1 - td;
    x = itd * itd * itd * p0.x +
        3 * itd * itd * td * p1.x +
        3 * itd * td * td * p2.x +
        td * td * td * p3.x;
    y = itd * itd * itd * p0.y +
        3 * itd * itd * td * p1.y +
        3 * itd * td * td * p2.y +
        td * td * td * p3.y;
    f(x, y, first_point);
    first_point = false;
  }
}

void bresenham(
    int x0, int y0, int x1, int y1,
    std::function<void(int x, int y)> f
) {
  if (std::abs(y1 - y0) < std::abs(x1 - x0)) {
    if (x0 > x1)
      internal::plot_line_low(x1, y1, x0, y0, f);
    else
      internal::plot_line_low(x0, y0, x1, y1, f);
  } else {
    if (y0 > y1)
      internal::plot_line_high(x1, y1, x0, y0, f);
    else
      internal::plot_line_high(x0, y0, x1, y1, f);
  }
}

} // namespace helpers
} // namespace hades