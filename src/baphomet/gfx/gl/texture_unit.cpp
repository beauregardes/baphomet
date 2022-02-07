#include "baphomet/gfx/gl/texture_unit.hpp"

#include "spdlog/spdlog.h"
#include "stb_image.h"

namespace baphomet::gl {

TextureUnit::TextureUnit(const std::string &path, bool retro) {
  stbi_set_flip_vertically_on_load(false);

  gen_id_();
  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);

  int w, h, comp;
  auto bytes = stbi_load(path.c_str(), &w, &h, &comp, 0);

  if (bytes) {
    width_ = w;
    height_ = h;

    GLenum format = GL_NONE;
    if (comp == 3)
      format = GL_RGB;
    else if (comp == 4)
      format = GL_RGBA;
    else
      spdlog::error("Can't handle images with comp '{}', only 3 or 4 channels supported", comp);

    if (format != GL_NONE) {
      glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
      glGenerateMipmap(GL_TEXTURE_2D);

      spdlog::debug("Loaded texture '{}' ({}x{})", path, width_, height_);
    }

    if (comp == 4) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int alpha_ch = (y * w * 4) + (x * 4) + 3;
          if (bytes[alpha_ch] < 255)
            fully_opaque_ = false;
        }
        if (!fully_opaque_)
          break;
      }
    }

  } else
    spdlog::error("Failed to load texture '{}': {}", path, stbi_failure_reason());

  stbi_image_free(bytes);
  unbind();
}

TextureUnit::TextureUnit(const std::filesystem::path &path, bool retro)
    : TextureUnit(path.string(), retro) {}

TextureUnit::~TextureUnit() {
  del_id_();
}

TextureUnit::TextureUnit(TextureUnit &&other) {
  id_ = other.id_;
  width_ = other.width_;
  height_ = other.height_;

  other.id_ = 0;
  other.width_ = 0;
  other.height_ = 0;
}

TextureUnit &TextureUnit::operator=(TextureUnit &&other) {
  if (this != &other) {
    del_id_();

    id_ = other.id_;
    width_ = other.width_;
    height_ = other.height_;

    other.id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
  }
  return *this;
}

GLuint TextureUnit::id() const {
  return id_;
}

void TextureUnit::bind(int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id_);
}

void TextureUnit::unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextureUnit::width() const {
  return width_;
}

GLuint TextureUnit::height() const {
  return height_;
}

bool TextureUnit::fully_opaque() const {
  return fully_opaque_;
}

void TextureUnit::gen_id_() {
  glGenTextures(1, &id_);
  spdlog::trace("Generated texture ({})", id_);
}

void TextureUnit::del_id_() {
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
    spdlog::trace("Deleted texture ({})", id_);
  }
}

} // namespace baphomet::gl
