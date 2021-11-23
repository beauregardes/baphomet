#include "gl/batching/batch.hpp"

namespace gl {

Batch::Batch(GladGLContext *ctx)
    : ctx_(ctx) {}

bool Batch::empty_opaque() {
    return opaque_vertices_->size() == 0;
}

bool Batch::empty_alpha() {
    return alpha_vertices_->size() == 0;
}

void Batch::clear() {
    opaque_vertices_->clear();
    alpha_vertices_->clear();
}

} // namespace gl