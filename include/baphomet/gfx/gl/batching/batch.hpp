#pragma once

#include "baphomet/gfx/gl/shader.hpp"
#include "baphomet/gfx/gl/vec_buffer.hpp"
#include "baphomet/gfx/gl/vertex_array.hpp"

#include "glm/glm.hpp"

#include <memory>

namespace baphomet::gl {

enum class BatchType { none, pixel, line, tri, rect, oval, lined, texture };

class Batch {
public:
  BatchType type{BatchType::none};

  Batch(std::size_t floats_per_vertex, BatchType type);

  virtual void clear();

  bool empty_opaque();
  bool empty_alpha();

  virtual std::size_t size_opaque();
  virtual std::size_t size_alpha();

  virtual std::size_t vertex_count_opaque();
  virtual std::size_t vertex_count_alpha();

  virtual void draw_opaque(float z_max, glm::mat4 projection) = 0;
  virtual void draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) = 0;

protected:
  std::size_t floats_per_vertex_{0};

  std::unique_ptr<Shader> shader_{nullptr};

  std::unique_ptr<VertexArray> opaque_vao_{nullptr};
  std::unique_ptr<VecBuffer<float>> opaque_vertices_{nullptr};

  std::unique_ptr<VertexArray> alpha_vao_{nullptr};
  std::unique_ptr<VecBuffer<float>> alpha_vertices_{nullptr};
};

} // namespace baphomet::gl
