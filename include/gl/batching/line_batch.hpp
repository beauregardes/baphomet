#ifndef GL_LINE_BATCH_HPP
#define GL_LINE_BATCH_HPP

#include "gl/batching/batch.hpp"

namespace gl {

class LineBatch : public Batch {
public:
  LineBatch(GladGLContext *ctx);
  ~LineBatch() = default;

  void add(
    float x0, float y0,
    float x1, float y1,
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
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_alpha_(
    float x0, float y0,
    float x1, float y1,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );
};

} // namespace gl

#endif //GL_LINE_BATCH_HPP
