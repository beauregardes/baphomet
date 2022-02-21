#pragma once

#include "baphomet/gfx/gl/batching/batch.hpp"

namespace baphomet::gl {

class LinedBatch : public Batch {
public:
  LinedBatch();
  ~LinedBatch() = default;

  void clear() override;

  void add_tri(
      float x0, float y0,
      float x1, float y1,
      float x2, float y2,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_rect(
      float x, float y,
      float w, float h,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_oval(
      float x, float y,
      float x_radius, float y_radius,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  std::size_t size_opaque() override;
  std::size_t size_alpha() override;

  std::size_t vertex_count_opaque() override;
  std::size_t vertex_count_alpha() override;

  void draw_opaque(float z_max, glm::mat4 projection) override;
  void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) override;

private:
  std::unique_ptr<VecBuffer<unsigned int>> opaque_indices_{nullptr};
  std::unique_ptr<VecBuffer<unsigned int>> alpha_indices_{nullptr};

  void check_initialize_opaque_();
  void check_initialize_alpha_();

  void add_tri_opaque_(
      float x0, float y0,
      float x1, float y1,
      float x2, float y2,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_tri_alpha_(
      float x0, float y0,
      float x1, float y1,
      float x2, float y2,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_rect_opaque_(
      float x, float y,
      float w, float h,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_rect_alpha_(
      float x, float y,
      float w, float h,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_oval_opaque_(
      float x, float y,
      float x_radius, float y_radius,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_oval_opaque_recurse_(
      float x, float y,
      float x_radius, float y_radius,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle,
      float x0, float y0, float a0,
      float x1, float y1, float a1
  );

  void add_oval_alpha_(
      float x, float y,
      float x_radius, float y_radius,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle
  );

  void add_oval_alpha_recurse_(
      float x, float y,
      float x_radius, float y_radius,
      float z,
      float r, float g, float b, float a,
      float cx, float cy, float angle,
      float x0, float y0, float a0,
      float x1, float y1, float a1
  );
};

} // namespace baphomet::gl
