#include "gl/batching/batch.hpp"

namespace gl {

Batch::Batch(GladGLContext *ctx)
    : ctx_(ctx) {}

bool Batch::empty_opaque() {
    return !opaque_vertices_ || opaque_vertices_->size() == 0;
}

bool Batch::empty_alpha() {
    return !alpha_vertices_ || alpha_vertices_->size() == 0;
}

void Batch::clear() {
    if (opaque_vertices_) opaque_vertices_->clear();
    if (alpha_vertices_)  alpha_vertices_->clear();
}

} // namespace gl