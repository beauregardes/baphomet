#include "gl/batching/batch.hpp"

namespace gl {

Batch::Batch(GladGLContext *ctx, std::size_t floats_per_vertex)
  : ctx_(ctx), floats_per_vertex_(floats_per_vertex) {}

bool Batch::empty_opaque() {
  return !opaque_vertices_ || opaque_vertices_->size() == 0;
}

bool Batch::empty_alpha() {
  return !alpha_vertices_ || alpha_vertices_->size() == 0;
}

std::size_t Batch::size_opaque() {
  return opaque_vertices_ ? opaque_vertices_->size() / floats_per_vertex_ : 0;
}

std::size_t Batch::size_alpha() {
  return alpha_vertices_ ? alpha_vertices_->size() / floats_per_vertex_ : 0;
}

void Batch::clear() {
  if (opaque_vertices_) opaque_vertices_->clear();
  if (alpha_vertices_)  alpha_vertices_->clear();
}

} // namespace gl