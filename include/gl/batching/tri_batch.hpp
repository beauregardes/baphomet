#ifndef GL_TRI_BATCH_HPP
#define GL_TRI_BATCH_HPP

#include "gl/batching/batch.hpp"

namespace gl {

class TriBatch : public Batch {
public:
  TriBatch(GladGLContext *ctx);
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
  void draw_alpha(float z_max, glm::mat4 projection) override;

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

} // namespace gl

#endif //GL_TRI_BATCH_HPP
