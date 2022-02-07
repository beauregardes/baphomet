#pragma once

#include "glm/glm.hpp"

#include <functional>

namespace baphomet {

struct Point {
  float x{0}, y{0};
};

struct Line {
  float x0{0}, y0{0};
  float x1{0}, y1{0};
};

struct Tri {
  float x0{0}, y0{0};
  float x1{0}, y1{0};
  float x2{0}, y2{0};
};

struct Rect {
  float x{0}, y{0};
  float w{0}, h{0};
};

struct Oval {
  float x{0}, y{0};
  float rad_x{0}, rad_y{0};
};

struct Circle {
  float x{0}, y{0};
  float rad{0};
};

namespace helpers {

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p3, int steps,
    std::function<void(double x, double y)> f
);

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p3, int steps,
    std::function<void(double x, double y, bool first_point)> f
);

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int steps,
    std::function<void(double x, double y)> f
);

void bezier(
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int steps,
    std::function<void(double x, double y, bool first_point)> f
);

void bresenham(
    int x0, int y0, int x1, int y1,
    std::function<void(int x, int y)> f
);

} // namespace helpers
} // namespace baphomet
