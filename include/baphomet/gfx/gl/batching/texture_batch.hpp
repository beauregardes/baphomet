#pragma once

#include "baphomet/gfx/gl/batching/batch.hpp"
#include "baphomet/gfx/gl/texture_unit.hpp"

namespace baphomet::gl {

class TextureBatch : public Batch {
public:
  TextureBatch(const std::unique_ptr<gl::TextureUnit> &texture_unit);
  ~TextureBatch() = default;

  bool fully_opaque();

  void add(
    float x, float y,
    float w, float h,
    float tx, float ty,
    float tw, float th,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  const std::unique_ptr<gl::TextureUnit> &texture_unit_;
  float x_px_unit_{0.0f}, y_px_unit_{0.0f};

  void add_opaque_(
    float x, float y,
    float w, float h,
    float tx, float ty,
    float tw, float th,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_alpha_(
    float x, float y,
    float w, float h,
    float tx, float ty,
    float tw, float th,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );
};

} // namespace gl
