#include "baphomet/gfx/internal/batch_set.hpp"

namespace baphomet {

BatchSet::BatchSet() {
  clear_batch_starts_();
}

void BatchSet::clear() {
  if (pixels) pixels->clear();
  if (lines)  lines->clear();
  if (lined)  lined->clear();
  if (tris)   tris->clear();
  if (rects)  rects->clear();
  if (ovals)  ovals->clear();
  for (auto &p : tex_batches_)
    p.second->clear();

  clear_batch_starts_();
  alpha_fns_.clear();
  last_batch_type_ = gl::BatchType::none;
  last_tex_name_ = "";

  z_level = 1.0f;
}

std::size_t BatchSet::pixel_vertex_count_opaque() const {
  return pixels ? pixels->vertex_count_opaque() : 0;
}

std::size_t BatchSet::line_vertex_count_opaque() const {
  return lines ? lines->vertex_count_opaque() : 0;
}

std::size_t BatchSet::tri_vertex_count_opaque() const {
  return tris ? tris->vertex_count_opaque() : 0;
}

std::size_t BatchSet::rect_vertex_count_opaque() const {
  return rects ? rects->vertex_count_opaque() : 0;
}

std::size_t BatchSet::oval_vertex_count_opaque() const {
  return ovals ? ovals->vertex_count_opaque() : 0;
}

std::size_t BatchSet::texture_vertex_count_opaque() const {
  std::size_t vertex_count{0};
  for (const auto &p : tex_batches_)
    vertex_count += p.second->vertex_count_alpha();
  return vertex_count;
}

std::size_t BatchSet::pixel_vertex_count_alpha() const {
  return pixels ? pixels->vertex_count_alpha() : 0;
}

std::size_t BatchSet::line_vertex_count_alpha() const {
  return lines ? lines->vertex_count_alpha() : 0;
}

std::size_t BatchSet::tri_vertex_count_alpha() const {
  return tris ? tris->vertex_count_alpha() : 0;
}

std::size_t BatchSet::rect_vertex_count_alpha() const {
  return rects ? rects->vertex_count_alpha() : 0;
}

std::size_t BatchSet::oval_vertex_count_alpha() const {
  return ovals ? ovals->vertex_count_alpha() : 0;
}

std::size_t BatchSet::texture_vertex_count_alpha() const {
  std::size_t vertex_count{0};
  for (const auto &p : tex_batches_)
    vertex_count += p.second->vertex_count_opaque();
  return vertex_count;
}

void BatchSet::create_texture_batch(const std::string &name, const std::unique_ptr<gl::TextureUnit> &tex_unit) {
  tex_batches_[name] = std::make_unique<gl::TextureBatch>(tex_unit);
  tex_batch_starts_[name] = 0;
}

void BatchSet::add_pixel(float x, float y, const baphomet::RGB &color) {
  if (!pixels)
    pixels = std::make_unique<gl::PixelBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::pixel);

  auto cv = color.vec4();
  pixels->add(x + 0.5f, y + 0.5f, z_level, cv.r, cv.g, cv.b, cv.a);
  z_level++;
}

void BatchSet::add_line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!lines)
    lines = std::make_unique<gl::LineBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::line);

  auto cv = color.vec4();
  lines->add(
      x0 + 0.5f, y0 + 0.5f,
      x1 + 0.5f, y1 + 0.5f,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!tris)
    tris = std::make_unique<gl::TriBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::tri);

  auto cv = color.vec4();
  tris->add(
      x0, y0,
      x1, y1,
      x2, y2,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!rects)
    rects = std::make_unique<gl::RectBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::rect);

  auto cv = color.vec4();
  rects->add(
      x, y,
      w, h,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!ovals)
    ovals = std::make_unique<gl::OvalBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::oval);

  auto cv = color.vec4();
  ovals->add(
      x + 0.5f, y + 0.5f,
      x_radius + 0.5f, y_radius + 0.5f,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx + 0.5f, cy + 0.5f, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_texture(const std::string &name, float x, float y, float w, float h, float tx, float ty, float tw, float th, float cx, float cy, float angle, const baphomet::RGB &color) {
  if (color.a < 255 || !tex_batches_[name]->fully_opaque())
    check_store_alpha_batch_(gl::BatchType::texture, name);

  auto cv = color.vec4();
  tex_batches_[name]->add(
      x, y, w, h,
      tx, ty, tw, th,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_lined_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!lined)
    lined = std::make_unique<gl::LinedBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::lined);

  auto cv = color.vec4();
  lined->add_tri(
      x0 + 0.5f, y0 + 0.5f,
      x1 + 0.5f, y1 + 0.5f,
      x2 + 0.5f, y2 + 0.5f,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_lined_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!lined)
    lined = std::make_unique<gl::LinedBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::lined);

  auto cv = color.vec4();
  lined->add_rect(
      x + 0.5f, y + 0.5f,
      w - 1, h - 1,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx, cy, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::add_lined_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  if (!lined)
    lined = std::make_unique<gl::LinedBatch>();
  if (color.a < 255)
    check_store_alpha_batch_(gl::BatchType::lined);

  auto cv = color.vec4();
  lined->add_oval(
      x + 0.5f, y + 0.5f,
      x_radius, y_radius,
      z_level,
      cv.r, cv.g, cv.b, cv.a,
      cx + 0.5f, cy + 0.5f, glm::radians(angle)
  );
  z_level++;
}

void BatchSet::draw_opaque(glm::mat4 projection) {
  for (auto &p : tex_batches_)
    p.second->draw_opaque(z_level, projection);
  if (ovals)  ovals->draw_opaque(z_level, projection);
  if (rects)  rects->draw_opaque(z_level, projection);
  if (tris)   tris->draw_opaque(z_level, projection);
  if (lined)  lined->draw_opaque(z_level, projection);
  if (lines)  lines->draw_opaque(z_level, projection);
  if (pixels) pixels->draw_opaque(z_level, projection);
}

void BatchSet::draw_alpha(glm::mat4 projection) {
  for (auto &f : alpha_fns_)
    f(z_level, projection);

  switch (last_batch_type_) {
    case gl::BatchType::pixel:
      pixels->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          pixels->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::line:
      lines->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          lines->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::lined:
      lined->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          lined->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::tri:
      tris->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          tris->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::rect:
      rects->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          rects->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::oval:
      ovals->draw_alpha(
          z_level,
          projection,
          batch_starts_[last_batch_type_],
          ovals->size_alpha() - batch_starts_[last_batch_type_]
      );
      break;

    case gl::BatchType::texture:
      tex_batches_[last_tex_name_]->draw_alpha(
          z_level,
          projection,
          tex_batch_starts_[last_tex_name_],
          tex_batches_[last_tex_name_]->size_alpha() - tex_batch_starts_[last_tex_name_]
      );
      break;

    case gl::BatchType::none:
      // We didn't draw any alpha geometry
      break;
  }
}

void BatchSet::clear_batch_starts_() {
  batch_starts_[gl::BatchType::pixel] = 0;
  batch_starts_[gl::BatchType::line] = 0;
  batch_starts_[gl::BatchType::lined] = 0;
  batch_starts_[gl::BatchType::tri] = 0;
  batch_starts_[gl::BatchType::rect] = 0;
  batch_starts_[gl::BatchType::oval] = 0;
  for (auto &p : tex_batches_)
    tex_batch_starts_[p.first] = 0;
}

void BatchSet::check_store_alpha_batch_(gl::BatchType current_type) {
  if (last_batch_type_ != gl::BatchType::none && last_batch_type_ != current_type)
    store_alpha_batch_();
  last_batch_type_ = current_type;
}

void BatchSet::check_store_alpha_batch_(gl::BatchType current_type, const std::string &current_tex_name) {
  if (last_batch_type_ != gl::BatchType::none && (last_batch_type_ != current_type || last_tex_name_ != current_tex_name))
    store_alpha_batch_();
  last_batch_type_ = current_type;
  last_tex_name_ = current_tex_name;
}

void BatchSet::store_alpha_batch_() {
  switch (last_batch_type_) {
    case gl::BatchType::pixel:
      alpha_fns_.emplace_back(std::bind(
          &gl::PixelBatch::draw_alpha,
          pixels.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          pixels->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = pixels->size_alpha();
      break;

    case gl::BatchType::line:
      alpha_fns_.emplace_back(std::bind(
          &gl::LineBatch::draw_alpha,
          lines.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          lines->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = lines->size_alpha();
      break;

    case gl::BatchType::lined:
      alpha_fns_.emplace_back(std::bind(
          &gl::LinedBatch::draw_alpha,
          lined.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          lined->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = lined->size_alpha();
      break;

    case gl::BatchType::tri:
      alpha_fns_.emplace_back(std::bind(
          &gl::TriBatch::draw_alpha,
          tris.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          tris->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = tris->size_alpha();
      break;

    case gl::BatchType::rect:
      alpha_fns_.emplace_back(std::bind(
          &gl::RectBatch::draw_alpha,
          rects.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          rects->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = rects->size_alpha();
      break;

    case gl::BatchType::oval:
      alpha_fns_.emplace_back(std::bind(
          &gl::OvalBatch::draw_alpha,
          ovals.get(),
          std::placeholders::_1, std::placeholders::_2,
          batch_starts_[last_batch_type_],
          ovals->size_alpha() - batch_starts_[last_batch_type_])
      );
      batch_starts_[last_batch_type_] = ovals->size_alpha();
      break;

    case gl::BatchType::texture:
      alpha_fns_.emplace_back(std::bind(
          &gl::TextureBatch::draw_alpha,
          tex_batches_[last_tex_name_].get(),
          std::placeholders::_1, std::placeholders::_2,
          tex_batch_starts_[last_tex_name_],
          tex_batches_[last_tex_name_]->size_alpha() - tex_batch_starts_[last_tex_name_]
      ));
      tex_batch_starts_[last_tex_name_] = tex_batches_[last_tex_name_]->size_alpha();
      break;
  }
}

} // namespace baphomet