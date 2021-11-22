#include "gl/batching/batch.hpp"

namespace gl {

Batch::Batch(GladGLContext *ctx)
    : ctx_(ctx) {}

void Batch::clear() {
    vertices_->clear();
}

} // namespace gl