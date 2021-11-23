#include "hades/batch_set.hpp"

namespace hades {

BatchSet::BatchSet(GladGLContext *ctx)
    : ctx_(ctx) {

    pixels = std::make_unique<gl::PixelBatch>(ctx_);
    lines = std::make_unique<gl::LineBatch>(ctx_);
    tris = std::make_unique<gl::TriBatch>(ctx_);
    rects = std::make_unique<gl::RectBatch>(ctx_);
    ovals = std::make_unique<gl::OvalBatch>(ctx_);
}

void BatchSet::clear() {
    pixels->clear();
    lines->clear();
    tris->clear();
    rects->clear();
    ovals->clear();
    for (auto &p : textures)
        p.second->clear();

    z_level = 1.0f;
}

void BatchSet::draw_opaque(glm::mat4 projection) {
    for (auto &p : textures)
        p.second->draw_opaque(z_level, projection);
    ovals->draw_opaque(z_level, projection);
    rects->draw_opaque(z_level, projection);
    tris->draw_opaque(z_level, projection);
    lines->draw_opaque(z_level, projection);
    pixels->draw_opaque(z_level, projection);
}

void BatchSet::draw_alpha(glm::mat4 projection) {
    for (auto &p : textures)
        p.second->draw_alpha(z_level, projection);
    ovals->draw_alpha(z_level, projection);
    rects->draw_alpha(z_level, projection);
    tris->draw_alpha(z_level, projection);
    lines->draw_alpha(z_level, projection);
    pixels->draw_alpha(z_level, projection);
}

} // namespace hades