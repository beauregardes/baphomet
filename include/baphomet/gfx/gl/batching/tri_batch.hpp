#pragma once

#include "baphomet/gfx/gl/batching/batch.hpp"

namespace baphomet::gl {

class TriBatch : public Batch {
public:
  TriBatch();
  ~TriBatch() = default;

  void add(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  void add_opaque_(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_alpha_(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );
};

} // namespace baphomet::gl
