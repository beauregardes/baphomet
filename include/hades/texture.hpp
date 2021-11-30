#ifndef HADES_TEXTURE_HPP
#define HADES_TEXTURE_HPP

#include "hades/color.hpp"
#include "hades/internal/batch_set.hpp"

namespace hades {

class Texture {
public:
  Texture(
    const std::unique_ptr<BatchSet> &bs,
    const std::string &name,
    GLuint width, GLuint height
  );

  GLuint w() const;
  GLuint h() const;

  void draw(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    float cx, float cy, float angle,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    float x, float y, float w, float h,
    float cx, float cy, float angle,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    float x, float y, float w, float h,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    float x, float y,
    float cx, float cy, float angle,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    float x, float y,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

protected:
  const std::unique_ptr<BatchSet> &bs_{nullptr};
  std::string name_{};

  GLuint width_{0}, height_{0};
};

} // namespace hades

#endif //HADES_TEXTURE_HPP
