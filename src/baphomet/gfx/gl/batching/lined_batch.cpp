#include "baphomet/gfx/gl/batching/lined_batch.hpp"

namespace baphomet::gl {

LinedBatch::LinedBatch() : Batch(10, BatchType::lined) {
  shader_ = ShaderBuilder("LineBatch")
      .vert_from_src(R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 in_trans;

out vec4 out_color;

uniform float z_max;
uniform mat4 projection;

void main() {
  float c = cos(in_trans.z);
  float s = sin(in_trans.z);
  float x = in_trans.x;
  float y = in_trans.y;
  float m30 = -x * c + y * s + x;
  float m31 = -x * s - y * c + y;
  mat4 trans = mat4(
    vec4(c,   s,   0.0, 0.0),
    vec4(-s,  c,   0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(m30, m31, 0.0, 1.0)
  );

  float z = -(z_max - in_pos.z) / (z_max + 1.0);
  gl_Position = projection * trans * vec4(in_pos.xy, z, 1.0);
  out_color = in_color;
}
    )glsl")
      .frag_from_src(R"glsl(
#version 330 core
in vec4 out_color;

out vec4 FragColor;

void main() {
  FragColor = out_color;
}
    )glsl")
      .link();
}

void LinedBatch::clear() {
  Batch::clear();
  opaque_indices_->clear();
  alpha_indices_->clear();
}

void LinedBatch::add_tri(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  if (a < 1.0f)
    add_tri_alpha_(x0, y0, x1, y1, x2, y2, z, r, g, b, a, cx, cy, angle);
  else
    add_tri_opaque_(x0, y0, x1, y1, x2, y2, z, r, g, b, a, cx, cy, angle);
}

void LinedBatch::add_rect(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  if (a < 1.0f)
    add_rect_alpha_(x, y, w, h, z, r, g, b, a, cx, cy, angle);
  else
    add_rect_opaque_(x, y, w, h, z, r, g, b, a, cx, cy, angle);
}

void LinedBatch::add_oval(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  if (a < 1.0f)
    add_oval_alpha_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle);
  else
    add_oval_opaque_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle);
}

std::size_t LinedBatch::size_opaque() {
  return opaque_indices_ ? opaque_indices_->size() : 0;
}

std::size_t LinedBatch::size_alpha() {
  return alpha_indices_ ? alpha_indices_->size() : 0;
}

std::size_t LinedBatch::vertex_count_opaque() {
  return size_opaque();
}

std::size_t LinedBatch::vertex_count_alpha() {
  return size_alpha();
}

void LinedBatch::draw_opaque(float z_max, glm::mat4 projection) {
  if (!empty_opaque()) {
    opaque_vertices_->sync();
    opaque_indices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65565);

    opaque_vao_->draw_elements(
        DrawMode::line_loop,
        opaque_indices_->size(),
        GL_UNSIGNED_INT,
        reinterpret_cast<void *>(opaque_indices_->front() * sizeof(unsigned int))
    );

    glDisable(GL_PRIMITIVE_RESTART);
  }
}

void LinedBatch::draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) {
  if (!empty_alpha()) {
    alpha_vertices_->sync();
    alpha_indices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65565);

    alpha_vao_->draw_elements(
        DrawMode::line_loop,
        count,
        GL_UNSIGNED_INT,
        reinterpret_cast<void *>(first * sizeof(unsigned int))
    );

    glDisable(GL_PRIMITIVE_RESTART);
  }
}

void LinedBatch::check_initialize_opaque_() {
  if (!opaque_vertices_) {
    opaque_vertices_ = std::make_unique<VecBuffer<float>>(
        floats_per_vertex_ * 2, false, gl::BufTarget::array, gl::BufUsage::dynamic_draw);
    opaque_indices_ = std::make_unique<VecBuffer<unsigned int>>(
        1, true, gl::BufTarget::element_array, gl::BufUsage::dynamic_draw);

    opaque_vao_ = std::make_unique<VertexArray>();
    opaque_vao_->attrib_pointer(opaque_vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
        {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });

    opaque_vao_->indices(opaque_indices_.get());
  }
}

void LinedBatch::check_initialize_alpha_() {
  if (!alpha_vertices_) {
    alpha_vertices_ = std::make_unique<VecBuffer<float>>(
        floats_per_vertex_ * 2, false, gl::BufTarget::array, gl::BufUsage::dynamic_draw);
    alpha_indices_ = std::make_unique<VecBuffer<unsigned int>>(
        1, false, gl::BufTarget::element_array, gl::BufUsage::dynamic_draw);

    alpha_vao_ = std::make_unique<VertexArray>();
    alpha_vao_->attrib_pointer(alpha_vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
        {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });

    alpha_vao_->indices(alpha_indices_.get());
  }
}

void LinedBatch::add_tri_opaque_(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_opaque_();

  unsigned int base = opaque_vertices_->size() / floats_per_vertex_;
  opaque_indices_->add({base, base + 1, base + 2, 65565});

  opaque_vertices_->add({
      x0, y0, z, r, g, b, a, cx, cy, angle,
      x1, y1, z, r, g, b, a, cx, cy, angle,
      x2, y2, z, r, g, b, a, cx, cy, angle
  });
}

void LinedBatch::add_tri_alpha_(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_alpha_();

  unsigned int base = alpha_vertices_->size() / floats_per_vertex_;
  alpha_indices_->add({base, base + 1, base + 2, 65565});

  alpha_vertices_->add({
      x0, y0, z, r, g, b, a, cx, cy, angle,
      x1, y1, z, r, g, b, a, cx, cy, angle,
      x2, y2, z, r, g, b, a, cx, cy, angle
  });
}

void LinedBatch::add_rect_opaque_(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_opaque_();

  unsigned int base = opaque_vertices_->size() / floats_per_vertex_;
  opaque_indices_->add({base, base + 1, base + 2, base + 3, 65565});

  opaque_vertices_->add({
      x,     y,     z, r, g, b, a, cx, cy, angle,
      x + w, y,     z, r, g, b, a, cx, cy, angle,
      x + w, y + h, z, r, g, b, a, cx, cy, angle,
      x,     y + h, z, r, g, b, a, cx, cy, angle
  });
}

void LinedBatch::add_rect_alpha_(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_alpha_();

  unsigned int base = alpha_vertices_->size() / floats_per_vertex_;
  alpha_indices_->add({base, base + 1, base + 2, base + 3, 65565});

  alpha_vertices_->add({
      x,     y,     z, r, g, b, a, cx, cy, angle,
      x + w, y,     z, r, g, b, a, cx, cy, angle,
      x + w, y + h, z, r, g, b, a, cx, cy, angle,
      x,     y + h, z, r, g, b, a, cx, cy, angle
  });
}

void LinedBatch::add_oval_opaque_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_opaque_();
}

void LinedBatch::add_oval_opaque_recurse_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle,
    float x0, float y0, float a0,
    float x1, float y1, float a1
) {

}

void LinedBatch::add_oval_alpha_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_alpha_();
}

void LinedBatch::add_oval_alpha_recurse_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle,
    float x0, float y0, float a0,
    float x1, float y1, float a1
) {

}

} // namespace baphomet::gl
