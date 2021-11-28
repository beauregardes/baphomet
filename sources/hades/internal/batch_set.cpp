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

std::size_t BatchSet::pixel_vertex_count_alpha() const {
  return pixels ? pixels->size_alpha() : 0;
}

std::size_t BatchSet::line_vertex_count_alpha() const {
  return lines ? lines->size_alpha() : 0;
}

std::size_t BatchSet::tri_vertex_count_alpha() const {
  return tris ? tris->size_alpha() : 0;
}

std::size_t BatchSet::rect_vertex_count_alpha() const {
  return rects ? rects->size_alpha() : 0;
}

std::size_t BatchSet::oval_vertex_count_alpha() const {
  return ovals ? ovals->size_alpha() : 0;
}

std::size_t BatchSet::texture_vertex_count_alpha() const {
  std::size_t vertex_count{0};
  for (const auto &p : textures)
    vertex_count += p.second->size_opaque();
  return vertex_count;
}

std::size_t BatchSet::pixel_vertex_count_opaque() const {
  return pixels ? pixels->size_opaque() : 0;
}

std::size_t BatchSet::line_vertex_count_opaque() const {
  return lines ? lines->size_opaque() : 0;
}

std::size_t BatchSet::tri_vertex_count_opaque() const {
  return tris ? tris->size_opaque() : 0;
}

std::size_t BatchSet::rect_vertex_count_opaque() const {
  return rects ? rects->size_opaque() : 0;
}

std::size_t BatchSet::oval_vertex_count_opaque() const {
  return ovals ? ovals->size_opaque() : 0;
}

std::size_t BatchSet::texture_vertex_count_opaque() const {
  std::size_t vertex_count{0};
  for (const auto &p : textures)
    vertex_count += p.second->size_alpha();
  return vertex_count;
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