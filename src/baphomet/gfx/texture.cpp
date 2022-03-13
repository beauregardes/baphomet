#include "baphomet/gfx/texture.hpp"

namespace baphomet {

Texture::Texture(
    TexRenderFunc render_func,
    const std::string &name,
    GLuint width, GLuint height
) : name_(name), width_(width), height_(height), render_func_(render_func) {}

GLuint Texture::w() const {
  return width_;
}

GLuint Texture::h() const {
  return height_;
}

void Texture::draw(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    float cx, float cy, float angle,
    const baphomet::RGB &color
) {
  render_func_(
      x, y, w, h,
      tx, ty, tw, th,
      cx, cy, angle,
      color
  );
}

void Texture::draw(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    const baphomet::RGB &color
) {
  draw(x, y, w, h, tx, ty, tw, th, 0.0f, 0.0f, 0.0f, color);
}

void Texture::draw(
    float x, float y, float w, float h,
    float cx, float cy, float angle,
    const baphomet::RGB &color
) {
  draw(x, y, w, h, 0.0f, 0.0f, width_, height_, cx, cy, angle, color);
}

void Texture::draw(
    float x, float y, float w, float h,
    const baphomet::RGB &color
) {
  draw(x, y, w, h, 0.0f, 0.0f, width_, height_, 0.0f, 0.0f, 0.0f, color);
}

void Texture::draw(
    float x, float y,
    float cx, float cy, float angle,
    const baphomet::RGB &color
) {
  draw(x, y, width_, height_, 0.0f, 0.0f, width_, height_, cx, cy, angle, color);
}

void Texture::draw(
    float x, float y,
    const baphomet::RGB &color
) {
  draw(x, y, width_, height_, 0.0f, 0.0f, width_, height_, 0.0f, 0.0f, 0.0f, color);
}

} // namespace baphomet
