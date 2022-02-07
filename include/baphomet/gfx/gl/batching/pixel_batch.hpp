#pragma once

#include "baphomet/gfx/gl/batching/batch.hpp"

namespace baphomet::gl {

class PixelBatch : public Batch {
public:
  PixelBatch();
  ~PixelBatch() = default;

  void add(
    float x, float y,
    float z,
    float r, float g, float b, float a
  );

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  void add_opaque_(
    float x, float y,
    float z,
    float r, float g, float b, float a
  );

  void add_alpha_(
    float x, float y,
    float z,
    float r, float g, float b, float a
  );
};

} // namespace baphomet::gl
