#pragma once

#include "baphomet/gfx/internal/batch_set.hpp"
#include "baphomet/gfx/color.hpp"

#include <functional>

namespace baphomet {

using TexRenderFunc = std::function<void(
    float, float, float, float,
    float, float, float, float,
    float, float, float,
    const baphomet::RGB &
)>;

class Texture {
public:
  Texture(
      TexRenderFunc render_func,
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
  std::string name_{};
  GLuint width_{0}, height_{0};

  TexRenderFunc render_func_;
};

} // namespace baphomet