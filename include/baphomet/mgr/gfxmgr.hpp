#pragma once

#include "baphomet/app/internal/resource_loader.hpp"
#include "baphomet/gfx/font/cp437.hpp"
#include "baphomet/gfx/gl/context_enums.hpp"
#include "baphomet/gfx/internal/batch_set.hpp"
#include "baphomet/gfx/color.hpp"
#include "baphomet/gfx/render_target.hpp"
#include "baphomet/gfx/spritesheet.hpp"
#include "baphomet/gfx/texture.hpp"
#include "baphomet/util/shapes.hpp"

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace baphomet {

class GfxMgr {
  friend class Application;

public:
  GfxMgr(float width, float height);
  ~GfxMgr() = default;

  GfxMgr(const GfxMgr &) = delete;
  GfxMgr &operator=(const GfxMgr &) = delete;

  GfxMgr(GfxMgr &&other) noexcept;
  GfxMgr &operator=(GfxMgr &&other) noexcept;

  /*****************
   * OPENGL CONTROL
   */

  void clear_color(const baphomet::RGB &color);
  void clear(gl::ClearMask mask = gl::ClearMask::color | gl::ClearMask::depth);

  /*************
   * PRIMITIVES
   */

  void pixel(float x, float y, const baphomet::RGB &color);
  void pixel(Point p, const baphomet::RGB &color);

  void line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float cx, float cy, float angle);
  void line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float angle);
  void line(float x0, float y0, float x1, float y1, const baphomet::RGB &color);
  void line(Line l, const baphomet::RGB &color, float cx, float cy, float angle);
  void line(Line l, const baphomet::RGB &color, float angle);
  void line(Line l, const baphomet::RGB &color);

  // ********** FILLED ***********

  void fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float angle);
  void fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color);
  void fill_tri(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_tri(float x, float y, float radius, const baphomet::RGB &color, float angle);
  void fill_tri(float x, float y, float radius, const baphomet::RGB &color);
  void fill_tri(Tri t, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_tri(Tri t, const baphomet::RGB &color, float angle);
  void fill_tri(Tri t, const baphomet::RGB &color);
  void fill_tri(Point origin, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_tri(Point origin, float radius, const baphomet::RGB &color, float angle);
  void fill_tri(Point origin, float radius, const baphomet::RGB &color);

  void fill_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_rect(float x, float y, float w, float h, const baphomet::RGB &color, float angle);
  void fill_rect(float x, float y, float w, float h, const baphomet::RGB &color);
  void fill_rect(Rect r, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_rect(Rect r, const baphomet::RGB &color, float angle);
  void fill_rect(Rect r, const baphomet::RGB &color);

  void fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float angle);
  void fill_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color);
  void fill_oval(Oval o, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_oval(Oval o, const baphomet::RGB &color, float angle);
  void fill_oval(Oval o, const baphomet::RGB &color);

  void fill_circle(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_circle(float x, float y, float radius, const baphomet::RGB &color, float angle);
  void fill_circle(float x, float y, float radius, const baphomet::RGB &color);
  void fill_circle(Circle c, const baphomet::RGB &color, float cx, float cy, float angle);
  void fill_circle(Circle c, const baphomet::RGB &color, float angle);
  void fill_circle(Circle c, const baphomet::RGB &color);

  // ********** LINED ***********

  void draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float angle);
  void draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color);
  void draw_tri(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_tri(float x, float y, float radius, const baphomet::RGB &color, float angle);
  void draw_tri(float x, float y, float radius, const baphomet::RGB &color);
  void draw_tri(Tri t, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_tri(Tri t, const baphomet::RGB &color, float angle);
  void draw_tri(Tri t, const baphomet::RGB &color);
  void draw_tri(Point origin, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_tri(Point origin, float radius, const baphomet::RGB &color, float angle);
  void draw_tri(Point origin, float radius, const baphomet::RGB &color);

  void draw_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_rect(float x, float y, float w, float h, const baphomet::RGB &color, float angle);
  void draw_rect(float x, float y, float w, float h, const baphomet::RGB &color);
  void draw_rect(Rect r, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_rect(Rect r, const baphomet::RGB &color, float angle);
  void draw_rect(Rect r, const baphomet::RGB &color);

  void draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float angle);
  void draw_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color);
  void draw_oval(Oval o, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_oval(Oval o, const baphomet::RGB &color, float angle);
  void draw_oval(Oval o, const baphomet::RGB &color);

  void draw_circle(float x, float y, float radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_circle(float x, float y, float radius, const baphomet::RGB &color, float angle);
  void draw_circle(float x, float y, float radius, const baphomet::RGB &color);
  void draw_circle(Circle c, const baphomet::RGB &color, float cx, float cy, float angle);
  void draw_circle(Circle c, const baphomet::RGB &color, float angle);
  void draw_circle(Circle c, const baphomet::RGB &color);

  /***********
   * TEXTURES
   */

  std::unique_ptr<Texture> load_texture(const std::string &path, bool retro = false);

  SpritesheetBuilder load_spritesheet(const std::string &path, bool retro = false);

  std::unique_ptr<CP437> load_cp437(const std::string &path, int char_w, int char_h, bool retro = false);

  /*****************
   * RENDER TARGETS
   */

  std::shared_ptr<RenderTarget> make_render_target(float x, float y, float w, float h, std::uint64_t weight);
  std::shared_ptr<RenderTarget> make_render_target(float x, float y, float w, float h);

  void push_render_target(std::shared_ptr<RenderTarget> &render_target);
  void pop_render_target(std::size_t count = 1);

private:
  std::unique_ptr<ResourceLoader> resource_loader{nullptr};

  std::vector<std::shared_ptr<RenderTarget>> render_targets_{};
  std::uint64_t next_render_target_weight_{1};

  std::stack<std::shared_ptr<RenderTarget>> render_stack_{};

  /*****************
   * OPENGL CONTROL
   */

  void enable_(gl::Capability cap);
  void disable_(gl::Capability cap);

  void depth_func_(gl::DepthFunc func);
  void depth_mask_(bool flag);

  void blend_func_(gl::BlendFunc src, gl::BlendFunc dst);

  void viewport_(int x, int y, int w, int h);

  void clip_control_(gl::ClipOrigin origin, gl::ClipDepth depth);

  void flush_();

  /*****************
   * RENDER TARGETS
   */

  void clear_render_targets_();
  void reset_to_base_render_target_();

  void draw_render_targets_(
      GLsizei window_width, GLsizei window_height,
      glm::mat4 projection
  );
};

} // namespace baphomet
