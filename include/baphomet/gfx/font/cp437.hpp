#pragma once

#include "baphomet/gfx/texture.hpp"
#include "baphomet/util/shapes.hpp"

namespace baphomet {

class CP437 : public Texture {
public:
  CP437(
      const std::unique_ptr<BatchSet> &bs,
      const std::string &name,
      GLuint width, GLuint height,
      GLuint char_w, GLuint char_h
  );

  GLuint char_w() const;
  GLuint char_h() const;

  void render(float x, float y, const std::string &text);
  void render(float x, float y, const baphomet::RGB &color, const std::string &text);

  void render(float x, float y, float scale, const std::string &text);
  void render(float x, float y, float scale, const baphomet::RGB &color, const std::string &text);

  Rect calc_text_bounds(float x, float y, const std::string &text);
  Rect calc_text_bounds(float x, float y, float scale, const std::string &text);

private:
  GLuint char_w_{0}, char_h_{0};

  void render_(float x, float y, float scale, const baphomet::RGB &color, const std::string &text);
};

} // namespace baphomet
