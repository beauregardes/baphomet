#include "baphomet/gfx/texture.hpp"

namespace baphomet {

Texture::Texture(
    const std::unique_ptr<BatchSet> &bs,
    const std::string &name,
    GLuint width, GLuint height
) : bs_(bs), name_(name), width_(width), height_(height) {}

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
  bs_->add_texture(
      name_,
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
