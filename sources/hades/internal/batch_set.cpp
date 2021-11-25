#include "hades/internal/batch_set.hpp"

namespace hades {

void BatchSet::clear() {
    if (pixels) pixels->clear();
    if (lines)  lines->clear();
    if (tris)   tris->clear();
    if (rects)  rects->clear();
    if (ovals)  ovals->clear();
    for (auto &p : textures)
        p.second->clear();

    z_level = 1.0f;
}

void BatchSet::draw_opaque(glm::mat4 projection) {
    for (auto &p : textures)
        p.second->draw_opaque(z_level, projection);
    if (ovals)  ovals->draw_opaque(z_level, projection);
    if (rects)  rects->draw_opaque(z_level, projection);
    if (tris)   tris->draw_opaque(z_level, projection);
    if (lines)  lines->draw_opaque(z_level, projection);
    if (pixels) pixels->draw_opaque(z_level, projection);
}

void BatchSet::draw_alpha(glm::mat4 projection) {
    for (auto &p : textures)
        p.second->draw_alpha(z_level, projection);
    if (ovals)  ovals->draw_alpha(z_level, projection);
    if (rects)  rects->draw_alpha(z_level, projection);
    if (tris)   tris->draw_alpha(z_level, projection);
    if (lines)  lines->draw_alpha(z_level, projection);
    if (pixels) pixels->draw_alpha(z_level, projection);
}

} // namespace hades