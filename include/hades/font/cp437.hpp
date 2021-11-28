#ifndef HADES_CP437_HPP
#define HADES_CP437_HPP

#include "hades/texture.hpp"

#include "fmt/format.h"

namespace hades {

class CP437 : public Texture {
public:
  CP437(
    const std::unique_ptr<gl::TextureBatch> &batch,
    GLuint width, GLuint height,
    GLuint char_w, GLuint char_h,
    float &z_level
  );

  GLuint char_w() const;
  GLuint char_h() const;

  template<typename S, typename... Args>
  void render(float x, float y, const S &format, Args &&... args);

  template<typename S, typename... Args>
  void render(float x, float y, const hades::RGB &color, const S &format, Args &&... args);

  template<typename S, typename... Args>
  void render(float x, float y, float scale, const S &format, Args &&... args);

  template<typename S, typename... Args>
  void render(float x, float y, float scale, const hades::RGB &color, const S &format, Args &&... args);

private:
  GLuint char_w_{0}, char_h_{0};

  void render_(float x, float y, float scale, const hades::RGB &color, fmt::string_view format, fmt::format_args args);
};

template<typename S, typename... Args>
void CP437::render(float x, float y, const S &format, Args &&... args) {
  render_(x, y, 1.0f, hades::rgb(0xffffff), format, fmt::make_args_checked<Args...>(format, args...));
}

template<typename S, typename... Args>
void CP437::render(float x, float y, const hades::RGB &color, const S &format, Args &&... args) {
  render_(x, y, 1.0f, color, format, fmt::make_args_checked<Args...>(format, args...));
}

template<typename S, typename... Args>
void CP437::render(float x, float y, float scale, const S &format, Args &&... args) {
  render_(x, y, scale, hades::rgb(0xffffff), format, fmt::make_args_checked<Args...>(format, args...));
}

template<typename S, typename... Args>
void CP437::render(float x, float y, float scale, const hades::RGB &color, const S &format, Args &&... args) {
  render_(x, y, scale, color, format, fmt::make_args_checked<Args...>(format, args...));
}

} // namespace hades

#endif //HADES_CP437_HPP
