#include "hades/texture.hpp"

namespace hades {

Texture::Texture(const std::unique_ptr<gl::TextureBatch> &batch, GLuint width, GLuint height, float &z_level)
  : batch_(batch), z_level_(z_level), width_(width), height_(height) {}

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
  const hades::RGB &color
) {
  auto cv = color.vec4();
  batch_->add(
    x, y,
    w, h,
    tx, ty,
    tw, th,
    z_level_,
    cv.r, cv.g, cv.b, cv.a,
    cx, cy,
    glm::radians(angle)
  );
  z_level_++;
}

void Texture::draw(
  float x, float y, float w, float h,
  float tx, float ty, float tw, float th,
  const hades::RGB &color
) {
  draw(x, y, w, h, tx, ty, tw, th, 0.0f, 0.0f, 0.0f, color);
}

void Texture::draw(
  float x, float y, float w, float h,
  float cx, float cy, float angle,
  const hades::RGB &color
) {
  draw(x, y, w, h, 0.0f, 0.0f, width_, height_, cx, cy, angle, color);
}

void Texture::draw(
  float x, float y, float w, float h,
  const hades::RGB &color
) {
  draw(x, y, w, h, 0.0f, 0.0f, width_, height_, 0.0f, 0.0f, 0.0f, color);
}

void Texture::draw(
  float x, float y,
  float cx, float cy, float angle,
  const hades::RGB &color
) {
  draw(x, y, width_, height_, 0.0f, 0.0f, width_, height_, cx, cy, angle, color);
}

void Texture::draw(
  float x, float y,
  const hades::RGB &color
) {
  draw(x, y, width_, height_, 0.0f, 0.0f, width_, height_, 0.0f, 0.0f, 0.0f, color);
}

} // namespace hades