#include "baphomet/gfx/gl/batching/rect_batch.hpp"

namespace baphomet::gl {

RectBatch::RectBatch() : Batch(10, BatchType::rect) {
  shader_ = ShaderBuilder("RectBatch")
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

void RectBatch::add(
  float x, float y,
  float w, float h,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (a < 1.0f)
    add_alpha_(x, y, w, h, z, r, g, b, a, cx, cy, angle);
  else
    add_opaque_(x, y, w, h, z, r, g, b, a, cx, cy, angle);
}

void RectBatch::draw_opaque(float z_max, glm::mat4 projection) {
  if (!empty_opaque()) {
    opaque_vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    opaque_vao_->draw_arrays(
      DrawMode::triangles,
      opaque_vertices_->front() / floats_per_vertex_,
      opaque_vertices_->size() / floats_per_vertex_
    );
  }
}

void RectBatch::draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) {
  if (!empty_alpha()) {
    alpha_vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    alpha_vao_->draw_arrays(
      DrawMode::triangles,
      first / floats_per_vertex_,
      count / floats_per_vertex_
    );
  }
}

void RectBatch::add_opaque_(
  float x, float y,
  float w, float h,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (!opaque_vertices_) {
    opaque_vertices_ = std::make_unique<VecBuffer<float>>(
      floats_per_vertex_ * 6, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    opaque_vao_ = std::make_unique<VertexArray>();
    opaque_vao_->attrib_pointer(opaque_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
      {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
  }

  opaque_vertices_->add({
    x,   y,   z, r, g, b, a, cx, cy, angle,
    x + w, y,   z, r, g, b, a, cx, cy, angle,
    x + w, y + h, z, r, g, b, a, cx, cy, angle,
    x,   y,   z, r, g, b, a, cx, cy, angle,
    x + w, y + h, z, r, g, b, a, cx, cy, angle,
    x,   y + h, z, r, g, b, a, cx, cy, angle
  });
}

void RectBatch::add_alpha_(
  float x, float y,
  float w, float h,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (!alpha_vertices_) {
    alpha_vertices_ = std::make_unique<VecBuffer<float>>(
      floats_per_vertex_ * 6, false, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    alpha_vao_ = std::make_unique<VertexArray>();
    alpha_vao_->attrib_pointer(alpha_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
      {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
  }

  alpha_vertices_->add({
    x,   y,   z, r, g, b, a, cx, cy, angle,
    x + w, y,   z, r, g, b, a, cx, cy, angle,
    x + w, y + h, z, r, g, b, a, cx, cy, angle,
    x,   y,   z, r, g, b, a, cx, cy, angle,
    x + w, y + h, z, r, g, b, a, cx, cy, angle,
    x,   y + h, z, r, g, b, a, cx, cy, angle
  });
}

} // namespace baphomet::gl
