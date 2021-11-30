#ifndef GL_RECT_BATCH_HPP
#define GL_RECT_BATCH_HPP

#include "gl/batching/batch.hpp"

namespace gl {

class RectBatch : public Batch {
public:
  RectBatch(GladGLContext *ctx);
  ~RectBatch() = default;

  void add(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  void add_opaque_(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_alpha_(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );
};

} // namespace gl

#endif //GL_RECT_BATCH_HPP
