#pragma once

#include "glad/gl.h"

#include <filesystem>
#include <string>

namespace baphomet::gl {

class TextureUnit {
public:
  TextureUnit(const std::string &path, bool retro = false);
  TextureUnit(const std::filesystem::path &path, bool retro = false);

  ~TextureUnit();

  TextureUnit(const TextureUnit &) = delete;
  TextureUnit &operator=(const TextureUnit &) = delete;

  TextureUnit(TextureUnit &&other);
  TextureUnit &operator=(TextureUnit &&other);

  GLuint id() const;

  void bind(int unit = 0);
  void unbind();

  GLuint width() const;
  GLuint height() const;

  bool fully_opaque() const;

private:
  GLuint id_{0};
  GLuint width_{0}, height_{0};

  bool fully_opaque_{true};

  void gen_id_();
  void del_id_();
};

} // namespace baphomet::gl
