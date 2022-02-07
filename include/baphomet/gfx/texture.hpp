#pragma once

#include "baphomet/gfx/internal/batch_set.hpp"
#include "baphomet/gfx/color.hpp"

namespace baphomet {

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
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      float x, float y, float w, float h,
      float tx, float ty, float tw, float th,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      float x, float y, float w, float h,
      float cx, float cy, float angle,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      float x, float y, float w, float h,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      float x, float y,
      float cx, float cy, float angle,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      float x, float y,
      const baphomet::RGB &color = rgb(0xffffff)
  );

protected:
  const std::unique_ptr<BatchSet> &bs_{nullptr};
  std::string name_{};

  GLuint width_{0}, height_{0};
};

} // namespace baphomet