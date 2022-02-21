#pragma once

#include "baphomet/gfx/gl/batching/line_batch.hpp"
#include "baphomet/gfx/gl/batching/lined_batch.hpp"
#include "baphomet/gfx/gl/batching/oval_batch.hpp"
#include "baphomet/gfx/gl/batching/pixel_batch.hpp"
#include "baphomet/gfx/gl/batching/rect_batch.hpp"
#include "baphomet/gfx/gl/batching/texture_batch.hpp"
#include "baphomet/gfx/gl/batching/tri_batch.hpp"
#include "baphomet/gfx/color.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace baphomet {

class BatchSet {
public:
  BatchSet();

  void clear();

  std::size_t pixel_vertex_count_opaque() const;
  std::size_t line_vertex_count_opaque() const;
  std::size_t tri_vertex_count_opaque() const;
  std::size_t rect_vertex_count_opaque() const;
  std::size_t oval_vertex_count_opaque() const;
  std::size_t texture_vertex_count_opaque() const;

  std::size_t pixel_vertex_count_alpha() const;
  std::size_t line_vertex_count_alpha() const;
  std::size_t tri_vertex_count_alpha() const;
  std::size_t rect_vertex_count_alpha() const;
  std::size_t oval_vertex_count_alpha() const;
  std::size_t texture_vertex_count_alpha() const;

  void create_texture_batch(const std::string &name, const std::unique_ptr<gl::TextureUnit> &tex_unit);

  void add_pixel(float x, float y, const baphomet::RGB &color);
  void add_line(float x0, float y0, float x1, float y1, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_texture(const std::string &name, float x, float y, float w, float h, float tx, float ty, float tw, float th, float cx, float cy, float angle, const baphomet::RGB &color);

  void add_lined_tri(float x0, float y0, float x1, float y1, float x2, float y2, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_lined_rect(float x, float y, float w, float h, const baphomet::RGB &color, float cx, float cy, float angle);
  void add_lined_oval(float x, float y, float x_radius, float y_radius, const baphomet::RGB &color, float cx, float cy, float angle);

  void draw_opaque(glm::mat4 projection);
  void draw_alpha(glm::mat4 projection);

private:
  float z_level{1.0f};

  std::unique_ptr <gl::PixelBatch> pixels{nullptr};
  std::unique_ptr <gl::LineBatch> lines{nullptr};
  std::unique_ptr <gl::LinedBatch> lined{nullptr};
  std::unique_ptr <gl::TriBatch> tris{nullptr};
  std::unique_ptr <gl::RectBatch> rects{nullptr};
  std::unique_ptr <gl::OvalBatch> ovals{nullptr};

  std::unordered_map <std::string, std::unique_ptr<gl::TextureBatch>> tex_batches_{};

  gl::BatchType last_batch_type_{gl::BatchType::none};
  std::string last_tex_name_{};
  std::vector<std::function<void(float, glm::mat4)>> alpha_fns_{};
  std::unordered_map<gl::BatchType, GLint> batch_starts_{};
  std::unordered_map<std::string ,GLint> tex_batch_starts_{};

  void clear_batch_starts_();
  void check_store_alpha_batch_(gl::BatchType current_type);
  void check_store_alpha_batch_(gl::BatchType current_type, const std::string &current_tex_name);
  void store_alpha_batch_();
};

} // namespace baphomet
