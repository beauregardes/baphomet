#include "baphomet/mgr/gfxmgr.hpp"

#include "baphomet/util/random.hpp"

#include <algorithm>

namespace baphomet {

GfxMgr::GfxMgr(float width, float height) {
  resource_loader = std::make_unique<ResourceLoader>();

  // create the default render target
  make_render_target(0, 0, width, height);
  push_render_target(render_targets_[0]);
}

GfxMgr::GfxMgr(GfxMgr &&other) noexcept {
  resource_loader = std::move(other.resource_loader);
  // TODO: Fix this
}

GfxMgr &GfxMgr::operator=(GfxMgr &&other) noexcept {
  if (this != &other) {
    resource_loader = std::move(other.resource_loader);
    // TODO: Fix this
  }
  return *this;
}

/*****************
 * OPENGL CONTROL
 */

void GfxMgr::clear_color(const baphomet::RGB &color) {
  glClearColor(
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f
  );
}

void GfxMgr::clear(gl::ClearMask mask) {
  glClear(unwrap(mask));
}

/*************
 * PRIMITIVES
 */

void GfxMgr::pixel(float x, float y, const baphomet::RGB &color) {
  render_stack_.top()->batches_->add_pixel(x, y, color);
}

void GfxMgr::pixel(Point p, const baphomet::RGB &color) {
  pixel(p.x, p.y, color);
}

void GfxMgr::line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_line(x0, y0, x1, y1, color, cx, cy, angle);
}

void GfxMgr::line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float angle) {
  line(x0, y0, x1, y1, color, (x0 + x1) / 2.0f, (y0 + y1) / 2.0f, angle);
}

void GfxMgr::line(float x0, float y0, float x1, float y1, const baphomet::RGB &color) {
  line(x0, y0, x1, y1, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::line(Line l, const baphomet::RGB &color, float cx, float cy, float angle) {
  line(l.x0, l.y0, l.x1, l.y1, color, cx, cy, angle);
}

void GfxMgr::line(Line l, const baphomet::RGB &color, float angle) {
  line(l.x0, l.y0, l.x1, l.y1, color, (l.x0 + l.x1) / 2.0f, (l.y0 + l.y1) / 2.0f, angle);
}

void GfxMgr::line(Line l, const baphomet::RGB &color) {
  line(l.x0, l.y0, l.x1, l.y1, color, 0.0f, 0.0f, 0.0f);
}

// ********** FILLED ***********

void GfxMgr::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_tri(x0, y0, x1, y1, x2, y2, color, cx, cy, angle);
}

void GfxMgr::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float angle) {
  fill_tri(x0, y0, x1, y1, x2, y2, color, (x0 + x1 + x2) / 3.0f, (y0 + y1 + y2) / 3.0f, angle);
}

void GfxMgr::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color) {
  fill_tri(x0, y0, x1, y1, x2, y2, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_tri(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      cx, cy, angle
  );
}

void GfxMgr::fill_tri(float x, float y, float radius, const baphomet::RGB &color, float angle) {
  fill_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      x, y, angle
  );
}

void GfxMgr::fill_tri(float x, float y, float radius, const baphomet::RGB &color) {
  fill_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      0.0f, 0.0f, 0.0f
  );
}

void GfxMgr::fill_tri(Tri t, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, cx, cy, angle);
}

void GfxMgr::fill_tri(Tri t, const baphomet::RGB &color, float angle) {
  fill_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, (t.x0 + t.x1 + t.x2) / 3.0f, (t.y0 + t.y1 + t.y2) / 3.0f, angle);
}

void GfxMgr::fill_tri(Tri t, const baphomet::RGB &color) {
  fill_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_tri(Point origin, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      cx, cy, angle
  );
}

void GfxMgr::fill_tri(Point origin, float radius, const baphomet::RGB &color, float angle) {
  fill_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      origin.x, origin.y, angle
  );
}

void GfxMgr::fill_tri(Point origin, float radius, const baphomet::RGB &color) {
  fill_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      0.0f, 0.0f, 0.0f
  );
}

void GfxMgr::fill_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_rect(x, y, w, h, color, cx, cy, angle);
}

void GfxMgr::fill_rect(float x, float y, float w, float h, const baphomet::RGB &color, float angle) {
  fill_rect(x, y, w, h, color, x + w / 2.0f, y + h / 2.0f, angle);
}

void GfxMgr::fill_rect(float x, float y, float w, float h, const baphomet::RGB &color) {
  fill_rect(x, y, w, h, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_rect(Rect r, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_rect(r.x, r.y, r.w, r.h, color, cx, cy, angle);
}

void GfxMgr::fill_rect(Rect r, const baphomet::RGB &color, float angle) {
  fill_rect(r.x, r.y, r.w, r.h, color, r.x + r.w / 2.0f, r.y + r.h / 2.0f, angle);
}

void GfxMgr::fill_rect(Rect r, const baphomet::RGB &color) {
  fill_rect(r.x, r.y, r.w, r.h, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_oval(x, y, x_radius, y_radius, color, cx, cy, angle);
}

void GfxMgr::fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float angle) {
  fill_oval(x, y, x_radius, y_radius, color, x, y, angle);
}

void GfxMgr::fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color) {
  fill_oval(x, y, x_radius, y_radius, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_oval(Oval o, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_oval(o.x, o.y, o.rad_x, o.rad_y, color, cx, cy, angle);
}

void GfxMgr::fill_oval(Oval o, const baphomet::RGB &color, float angle) {
  fill_oval(o.x, o.y, o.rad_x, o.rad_y, color, o.x, o.y, angle);
}

void GfxMgr::fill_oval(Oval o, const baphomet::RGB &color) {
  fill_oval(o.x, o.y, o.rad_x, o.rad_y, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_circle(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_oval(x, y, radius, radius, color, cx, cy, angle);
}

void GfxMgr::fill_circle(float x, float y, float radius, const baphomet::RGB &color, float angle) {
  fill_oval(x, y, radius, radius, color, x, y, angle);
}

void GfxMgr::fill_circle(float x, float y, float radius, const baphomet::RGB &color) {
  fill_oval(x, y, radius, radius, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::fill_circle(Circle c, const baphomet::RGB &color, float cx, float cy, float angle) {
  fill_oval(c.x, c.y, c.rad, c.rad, color, cx, cy, angle);
}

void GfxMgr::fill_circle(Circle c, const baphomet::RGB &color, float angle) {
  fill_oval(c.x, c.y, c.rad, c.rad, color, c.x, c.y, angle);
}

void GfxMgr::fill_circle(Circle c, const baphomet::RGB &color) {
  fill_oval(c.x, c.y, c.rad, c.rad, color, 0.0f, 0.0f, 0.0f);
}

// ********** LINED ***********

void GfxMgr::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_lined_tri(x0, y0, x1, y1, x2, y2, color, cx, cy, angle);
}

void GfxMgr::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float angle) {
  draw_tri(x0, y0, x1, y1, x0, y2, color, (x0 + x1 + x2) / 3.0f, (y0 + y1 + y2) / 3.0f, angle);
}

void GfxMgr::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color) {
  draw_tri(x0, y0, x1, y1, x2, y2, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_tri(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      cx, cy, angle
  );
}

void GfxMgr::draw_tri(float x, float y, float radius, const baphomet::RGB &color, float angle) {
  draw_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      x, y, angle
  );
}

void GfxMgr::draw_tri(float x, float y, float radius, const baphomet::RGB &color) {
  draw_tri(
      x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
      x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
      x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
      color,
      0.0f, 0.0f, 0.0f
  );
}

void GfxMgr::draw_tri(Tri t, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, cx, cy, angle);
}

void GfxMgr::draw_tri(Tri t, const baphomet::RGB &color, float angle) {
  draw_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, (t.x0 + t.x1 + t.x2) / 3.0f, (t.y0 + t.y1 + t.y2) / 3.0f, angle);
}

void GfxMgr::draw_tri(Tri t, const baphomet::RGB &color) {
  draw_tri(t.x0, t.y0, t.x1, t.y1, t.x2, t.y2, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_tri(Point origin, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      cx, cy, angle
  );
}

void GfxMgr::draw_tri(Point origin, float radius, const baphomet::RGB &color, float angle) {
  draw_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      origin.x, origin.y, angle
  );
}

void GfxMgr::draw_tri(Point origin, float radius, const baphomet::RGB &color) {
  draw_tri(
      origin.x + radius * std::cos(-glm::radians(90.0f)),  origin.y + radius * std::sin(-glm::radians(90.0f)),
      origin.x + radius * std::cos(-glm::radians(210.0f)), origin.y + radius * std::sin(-glm::radians(210.0f)),
      origin.x + radius * std::cos(-glm::radians(330.0f)), origin.y + radius * std::sin(-glm::radians(330.0f)),
      color,
      0.0f, 0.0f, 0.0f
  );
}

void GfxMgr::draw_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_lined_rect(x, y, w, h, color, cx, cy, angle);
}

void GfxMgr::draw_rect(float x, float y, float w, float h, const baphomet::RGB &color, float angle) {
  draw_rect(x, y, w, h, color, x + w / 2.0f, y + h / 2.0f, angle);
}

void GfxMgr::draw_rect(float x, float y, float w, float h, const baphomet::RGB &color) {
  draw_rect(x, y, w, h, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_rect(Rect r, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_rect(r.x, r.y, r.w, r.h, color, cx, cy, angle);
}

void GfxMgr::draw_rect(Rect r, const baphomet::RGB &color, float angle) {
  draw_rect(r.x, r.y, r.w, r.h, color, r.x + r.w / 2.0f, r.y + r.h / 2.0f, angle);
}

void GfxMgr::draw_rect(Rect r, const baphomet::RGB &color) {
  draw_rect(r.x, r.y, r.w, r.h, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_lined_oval(x, y, x_radius, y_radius, color, cx, cy, angle);
}

void GfxMgr::draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float angle) {
  draw_oval(x, y, x_radius, y_radius, color, x, y, angle);
}

void GfxMgr::draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color) {
  draw_oval(x, y, x_radius, y_radius, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_oval(Oval o, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_oval(o.x, o.y, o.rad_x, o.rad_y, color, cx, cy, angle);
}

void GfxMgr::draw_oval(Oval o, const baphomet::RGB &color, float angle) {
  draw_oval(o.x, o.y, o.rad_x, o.rad_y, color, o.x, o.y, angle);
}

void GfxMgr::draw_oval(Oval o, const baphomet::RGB &color) {
  draw_oval(o.x, o.y, o.rad_x, o.rad_y, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_circle(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle) {
  render_stack_.top()->batches_->add_lined_oval(x, y, radius, radius, color, cx, cy, angle);
}

void GfxMgr::draw_circle(float x, float y, float radius, const baphomet::RGB &color, float angle) {
  draw_circle(x, y, radius, color, x, y, angle);
}

void GfxMgr::draw_circle(float x, float y, float radius, const baphomet::RGB &color) {
  draw_circle(x, y, radius, color, 0.0f, 0.0f, 0.0f);
}

void GfxMgr::draw_circle(Circle c, const baphomet::RGB &color, float cx, float cy, float angle) {
  draw_oval(c.x, c.y, c.rad, c.rad, color, cx, cy, angle);
}

void GfxMgr::draw_circle(Circle c, const baphomet::RGB &color, float angle) {
  draw_oval(c.x, c.y, c.rad, c.rad, color, c.x, c.y, angle);
}

void GfxMgr::draw_circle(Circle c, const baphomet::RGB &color) {
  draw_oval(c.x, c.y, c.rad, c.rad, color, 0.0f, 0.0f, 0.0f);
}

/***********
 * TEXTURES
 */

std::unique_ptr<Texture> GfxMgr::load_texture(const std::string &path, bool retro) {
  auto name = rnd::base58(11);
  resource_loader->load_texture_unit(name, path, retro);

  auto &tex = resource_loader->get_texture_unit(name);
  render_stack_.top()->batches_->create_texture_batch(name, tex);

  return std::make_unique<Texture>(
      render_stack_.top()->batches_,
      name,
      tex->width(), tex->height()
  );
}

SpritesheetBuilder GfxMgr::load_spritesheet(const std::string &path, bool retro) {
  auto name = rnd::base58(11);
  resource_loader->load_texture_unit(name, path, retro);

  auto &tex = resource_loader->get_texture_unit(name);
  render_stack_.top()->batches_->create_texture_batch(name, tex);

  return SpritesheetBuilder(render_stack_.top()->batches_, name);
}

std::unique_ptr<CP437> GfxMgr::load_cp437(const std::string &path, int char_w, int char_h, bool retro) {
  auto name = rnd::base58(11);
  resource_loader->load_texture_unit(name, path, retro);

  auto &tex = resource_loader->get_texture_unit(name);
  render_stack_.top()->batches_->create_texture_batch(name, tex);

  return std::make_unique<CP437>(
      render_stack_.top()->batches_,
      name,
      tex->width(), tex->height(),
      char_w, char_h
  );
}

/*****************
 * RENDER TARGETS
 */

std::shared_ptr<RenderTarget> GfxMgr::make_render_target(float x, float y, float w, float h, std::uint64_t weight) {
  auto new_render_target = std::make_shared<RenderTarget>(
      rnd::base58(11),
      weight,
      x, y, w, h
  );

  // Find where this element should be inserted according to its weight
  auto it = std::upper_bound(
      render_targets_.begin(), render_targets_.end(),
      new_render_target,
      [](auto l, auto r) { return l->weight_ < r->weight_; }
  );
  render_targets_.insert(it, new_render_target);;

  return new_render_target;
}

std::shared_ptr<RenderTarget> GfxMgr::make_render_target(float x, float y, float w, float h) {
  return make_render_target(x, y, w, h, next_render_target_weight_++);
}

void GfxMgr::push_render_target(std::shared_ptr<RenderTarget> &render_target) {
  render_stack_.push(render_target);
  render_target->fbo_->bind();
}

void GfxMgr::pop_render_target(std::size_t count) {
  // TODO: don't allow popping everything off
  for (int i = 0; i < count; i++)
    render_stack_.pop();
  render_stack_.top()->fbo_->bind();
}

/*****************
 * OPENGL CONTROL
 */

void GfxMgr::enable_(gl::Capability cap) {
  glEnable(unwrap(cap));
}

void GfxMgr::disable_(gl::Capability cap) {
  glDisable(unwrap(cap));
}

void GfxMgr::depth_func_(gl::DepthFunc func) {
  glDepthFunc(unwrap(func));
}

void GfxMgr::depth_mask_(bool flag) {
  glDepthMask(flag ? GL_TRUE : GL_FALSE);
}

void GfxMgr::blend_func_(gl::BlendFunc src, gl::BlendFunc dst) {
  glBlendFunc(unwrap(src), unwrap(dst));
}

void GfxMgr::viewport_(int x, int y, int w, int h) {
  glViewport(x, y, w, h);
}

void GfxMgr::clip_control_(gl::ClipOrigin origin, gl::ClipDepth depth) {
  glClipControl(unwrap(origin), unwrap(depth));
}

void GfxMgr::flush_() {
  glFlush();
}

/*****************
 * RENDER TARGETS
 */

void GfxMgr::clear_render_targets_() {
//  window_batch_->clear();
  for (auto &rt : render_targets_)
    rt->batches_->clear();
}

void GfxMgr::reset_to_base_render_target_() {
  while (render_stack_.size() > 1)
    render_stack_.pop();
  render_stack_.top()->fbo_->bind();
}

void GfxMgr::draw_render_targets_(
    GLsizei window_width, GLsizei window_height,
    glm::mat4 projection
) {
  for (auto &rt : render_targets_) {
    rt->fbo_->bind();

    rt->batches_->draw_opaque(rt->projection_);

    enable_(gl::Capability::blend);
    depth_mask_(false);

    rt->batches_->draw_alpha(rt->projection_);

    depth_mask_(true);
    disable_(gl::Capability::blend);

    // Reset back to the default framebuffer
    // and fix the viewport, so we can draw on it
    rt->fbo_->unbind();
    glViewport(0, 0, window_width, window_height);

    // This needs to be done between each call
    clear(gl::ClearMask::depth);

    enable_(gl::Capability::blend);
    depth_mask_(false);

    rt->draw_(projection);

    depth_mask_(true);
    disable_(gl::Capability::blend);
  }
}

} // namespace baphomet
