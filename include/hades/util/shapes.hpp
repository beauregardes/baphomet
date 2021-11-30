#ifndef HADES_SHAPES_HPP
#define HADES_SHAPES_HPP

namespace hades {

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

} // namespace hades

#endif //HADES_SHAPES_HPP