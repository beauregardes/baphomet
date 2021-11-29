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

  void render(float x, float y, const std::string &text);
  void render(float x, float y, const hades::RGB &color, const std::string &text);

  void render(float x, float y, float scale, const std::string &text);
  void render(float x, float y, float scale, const hades::RGB &color, const std::string &text);

  glm::vec4 calc_text_bounds(float x, float y, const std::string &text);
  glm::vec4 calc_text_bounds(float x, float y, float scale, const std::string &text);

private:
  GLuint char_w_{0}, char_h_{0};

  void render_(float x, float y, float scale, const hades::RGB &color, const std::string &text);
};

} // namespace hades

#endif //HADES_CP437_HPP
