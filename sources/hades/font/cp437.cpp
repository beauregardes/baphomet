#include "hades/font/cp437.hpp"

namespace hades {

CP437::CP437(
  const std::unique_ptr<gl::TextureBatch> &batch,
  GLuint width, GLuint height,
  GLuint char_w, GLuint char_h,
  float &z_level
) : Texture(batch, width, height, z_level), char_w_(char_w), char_h_(char_h) {}

GLuint CP437::char_w() const {
  return char_w_;
}

GLuint CP437::char_h() const {
  return char_h_;
}

void CP437::render(float x, float y, const std::string &text) {
  render_(x, y, 1.0f, hades::rgb(0xffffff), text);
}

void CP437::render(float x, float y, const hades::RGB &color, const std::string &text) {
  render_(x, y, 1.0f, color, text);
}

void CP437::render(float x, float y, float scale, const std::string &text) {
  render_(x, y, scale, hades::rgb(0xffffff), text);
}

void CP437::render(float x, float y, float scale, const hades::RGB &color, const std::string &text) {
  render_(x, y, scale, color, text);
}

glm::vec4 CP437::calc_text_bounds(float x, float y, const std::string &text) {
  return calc_text_bounds(x, y, 1.0f, text);
}

glm::vec4 CP437::calc_text_bounds(float x, float y, float scale, const std::string &text) {
  glm::vec4 bounds{x, y, 0.0f, 0.0f};

  for (const auto &c : text) {
    if (bounds.w == 0.0f)
      bounds.w = char_h_;

    int idx = static_cast<int>(c);

    if (idx == 10) {
      bounds.w += scale * char_h_;
      continue;

    } else if (idx == 32) {
      bounds.z += scale * char_w_;
      continue;

    } else if (idx >= 33 && idx <= 126) {
      bounds.z += scale * char_w_;
    }
  }

  return bounds;
}

void CP437::render_(float x, float y, float scale, const hades::RGB &color, const std::string &text) {
  float curr_x = x, curr_y = y;
  for (const auto &c : text) {
    int idx = static_cast<int>(c);

    if (idx == 10) {
      curr_y += scale * char_h_;
      curr_x = x;
      continue;

    } else if (idx == 32) {
      curr_x += scale * char_w_;
      continue;

    } else if (idx >= 33 && idx <= 126) {
      float tx = char_w_ * (idx % 16);
      float ty = char_h_ * ((idx / 16) - 2);

      draw(
        curr_x, curr_y,
        scale * char_w_, scale * char_h_,
        tx, ty,
        char_w_, char_h_,
        color
      );
      z_level_--;

      curr_x += scale * char_w_;
    }
  }
  z_level_++;
}

} // namespace hades