#ifndef GL_OVAL_BATCH_HPP
#define GL_OVAL_BATCH_HPP

/* The tessellation strategy is described here:
 *   http://www.humus.name/index.php?page=News&ID=228

 * ...specifically the third option shown that is called "max area"
 * This seems to give very good performance, and means there is
 * not a bunch of hand-tuning done to handle how many vertices
 * the circle should have depending on the size, as it is 
 * recursive.
 * 
 * It may be a good idea to convert this to an iterative algo instead
 * of using recursion so there's no chance of overflowing the stack
 */

#include "gl/batching/batch.hpp"

namespace gl {

class OvalBatch : public Batch {
public:
  OvalBatch(GladGLContext *ctx);
  ~OvalBatch() = default;

  void add(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  void add_opaque_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_opaque_recurse_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle,
    float x0, float y0, float a0,
    float x1, float y1, float a1
  );

  void add_alpha_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
  );

  void add_alpha_recurse_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle,
    float x0, float y0, float a0,
    float x1, float y1, float a1
  );
};

} // namespace gl

#endif //GL_OVAL_BATCH_HPP
