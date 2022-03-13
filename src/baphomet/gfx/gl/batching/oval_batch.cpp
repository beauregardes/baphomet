#include "baphomet/gfx/gl/batching/oval_batch.hpp"

namespace baphomet::gl {

OvalBatch::OvalBatch() : Batch(10, BatchType::oval) {
  shader_ = ShaderBuilder("OvalBatch")
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
  FragColor = vec4(out_color.xyz * out_color.a, out_color.a);
}
    )glsl")
            .link();
}

void OvalBatch::add(
  float x, float y,
  float x_radius, float y_radius,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (a < 1.0f)
    add_alpha_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle);
  else
    add_opaque_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle);
}

void OvalBatch::draw_opaque(float z_max, glm::mat4 projection) {
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

void OvalBatch::draw_alpha(float z_max, glm::mat4 projection, GLint first, GLsizei count) {
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

void OvalBatch::add_opaque_(
  float x, float y,
  float x_radius, float y_radius,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (!opaque_vertices_) {
    opaque_vertices_ = std::make_unique<VecBuffer<float>>(
      floats_per_vertex_ * 12, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    opaque_vao_ = std::make_unique<VertexArray>();
    opaque_vao_->attrib_pointer(opaque_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
      {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
  }

  static float
    a0 = 0.0f,
    a1 = glm::radians(90.0f),
    a2 = glm::radians(180.0f),
    a3 = glm::radians(270.0f),
    a4 = glm::radians(360.0f);

  float
    x0 = x + x_radius * std::cos(a0),
    y0 = y + y_radius * std::sin(a0),
    x1 = x + x_radius * std::cos(a1),
    y1 = y + y_radius * std::sin(a1),
    x2 = x + x_radius * std::cos(a2),
    y2 = y + y_radius * std::sin(a2),
    x3 = x + x_radius * std::cos(a3),
    y3 = y + y_radius * std::sin(a3);

  opaque_vertices_->add({
    x0, y0, z, r, g, b, a, cx, cy, angle,
    x1, y1, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,

    x0, y0, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,
    x3, y3, z, r, g, b, a, cx, cy, angle
  });

  add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x1, y1, a1);
  add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
  add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x3, y3, a3);
  add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x3, y3, a3, x0, y0, a4);
}

void OvalBatch::add_opaque_recurse_(
  float x, float y,
  float x_radius, float y_radius,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle,
  float x0, float y0, float a0,
  float x1, float y1, float a1
) {
  float a2 = (a0 + a1) / 2.0f;
  float
    x2 = x + x_radius * std::cos(a2),
    y2 = y + y_radius * std::sin(a2);

  opaque_vertices_->add({
    x0, y0, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,
    x1, y1, z, r, g, b, a, cx, cy, angle
  });

  float dist_sq =
    ((((x0 + x1) / 2.0f) - x2) * (((x0 + x1) / 2.0f) - x2)) +
    ((((y0 + y1) / 2.0f) - y2) * (((y0 + y1) / 2.0f) - y2));
  if (dist_sq > 2.0f) {
    add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x2, y2, a2);
    add_opaque_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
  }
}

void OvalBatch::add_alpha_(
  float x, float y,
  float x_radius, float y_radius,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle
) {
  if (!alpha_vertices_) {
    alpha_vertices_ = std::make_unique<VecBuffer<float>>(
      floats_per_vertex_ * 12, false, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    alpha_vao_ = std::make_unique<VertexArray>();
    alpha_vao_->attrib_pointer(alpha_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
      {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
  }

  static float
    a0 = 0.0f,
    a1 = glm::radians(90.0f),
    a2 = glm::radians(180.0f),
    a3 = glm::radians(270.0f),
    a4 = glm::radians(360.0f);

  float
    x0 = x + x_radius * std::cos(a0),
    y0 = y + y_radius * std::sin(a0),
    x1 = x + x_radius * std::cos(a1),
    y1 = y + y_radius * std::sin(a1),
    x2 = x + x_radius * std::cos(a2),
    y2 = y + y_radius * std::sin(a2),
    x3 = x + x_radius * std::cos(a3),
    y3 = y + y_radius * std::sin(a3);

  alpha_vertices_->add({
    x0, y0, z, r, g, b, a, cx, cy, angle,
    x1, y1, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,

    x0, y0, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,
    x3, y3, z, r, g, b, a, cx, cy, angle
  });

  add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x1, y1, a1);
  add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
  add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x3, y3, a3);
  add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x3, y3, a3, x0, y0, a4);
}

void OvalBatch::add_alpha_recurse_(
  float x, float y,
  float x_radius, float y_radius,
  float z,
  float r, float g, float b, float a,
  float cx, float cy, float angle,
  float x0, float y0, float a0,
  float x1, float y1, float a1
) {
  float a2 = (a0 + a1) / 2.0f;
  float
    x2 = x + x_radius * std::cos(a2),
    y2 = y + y_radius * std::sin(a2);

  alpha_vertices_->add({
    x0, y0, z, r, g, b, a, cx, cy, angle,
    x2, y2, z, r, g, b, a, cx, cy, angle,
    x1, y1, z, r, g, b, a, cx, cy, angle
  });

  float dist_sq =
    ((((x0 + x1) / 2.0f) - x2) * (((x0 + x1) / 2.0f) - x2)) +
    ((((y0 + y1) / 2.0f) - y2) * (((y0 + y1) / 2.0f) - y2));
  if (dist_sq > 2.0f) {
    add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x2, y2, a2);
    add_alpha_recurse_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
  }
}

} // namespace baphomet::gl
