#include "baphomet/gfx/font/cp437.hpp"

namespace baphomet {

CP437::CP437(
    TexRenderFunc render_func,
    const std::string &name,
    GLuint width, GLuint height,
    GLuint char_w, GLuint char_h
) : Texture(render_func, name, width, height), char_w_(char_w), char_h_(char_h) {}

GLuint CP437::char_w() const {
  return char_w_;
}

GLuint CP437::char_h() const {
  return char_h_;
}

void CP437::render(float x, float y, const std::string &text) {
  render_(x, y, 1.0f, baphomet::rgb(0xffffff), text);
}

void CP437::render(float x, float y, const baphomet::RGB &color, const std::string &text) {
  render_(x, y, 1.0f, color, text);
}

void CP437::render(float x, float y, float scale, const std::string &text) {
  render_(x, y, scale, baphomet::rgb(0xffffff), text);
}

void CP437::render(float x, float y, float scale, const baphomet::RGB &color, const std::string &text) {
  render_(x, y, scale, color, text);
}

Rect CP437::calc_text_bounds(float x, float y, const std::string &text) {
  return calc_text_bounds(x, y, 1.0f, text);
}

Rect CP437::calc_text_bounds(float x, float y, float scale, const std::string &text) {
  Rect bounds{x, y, 0.0f, 0.0f};

  for (const auto &c : text) {
    if (bounds.h == 0.0f)
      bounds.h = char_h_;

    int idx = static_cast<int>(c);

    if (idx == 10) {
      bounds.h += scale * char_h_;
      continue;

    } else if (idx == 32) {
      bounds.w += scale * char_w_;
      continue;

    } else if (idx >= 33 && idx <= 126) {
      bounds.w += scale * char_w_;
    }
  }

  return bounds;
}

void CP437::render_(float x, float y, float scale, const baphomet::RGB &color, const std::string &text) {
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

      curr_x += scale * char_w_;
    }
  }
}

} // namespace baphomet
