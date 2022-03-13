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
  FragColor = vec4(out_color.xyz * out_color.a, out_color.a);
}
    )glsl")
      .link();
}

void LinedBatch::clear() {
  Batch::clear();
  if (opaque_indices_) opaque_indices_->clear();
  if (alpha_indices_)  alpha_indices_->clear();
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

void push_vertex(
    std::vector<float> &vertices,
    std::vector<unsigned int> &indices,
    unsigned int &base,
    float x, float y, float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  vertices.insert(vertices.end(), {x, y, z, r, g, b, a, cx, cy, angle});
  indices.push_back(base);
  base++;
}

void LinedBatch::add_oval_opaque_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_opaque_();

  std::vector<float> vertices{};
  std::vector<unsigned int> indices{};
  unsigned int base = opaque_vertices_->size() / floats_per_vertex_;

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

  push_vertex(vertices, indices, base, x0, y0, z, r, g, b, a, cx, cy, angle);
  add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x1, y1, a1);

  push_vertex(vertices, indices, base, x1, y1, z, r, g, b, a, cx, cy, angle);
  add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);

  push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
  add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x3, y3, a3);

  push_vertex(vertices, indices, base, x3, y3, z, r, g, b, a, cx, cy, angle);
  add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x3, y3, a3, x0, y0, a4);

  indices.push_back(65565);
  opaque_indices_->add(indices);
  opaque_vertices_->add(vertices);
}

void LinedBatch::add_oval_opaque_recurse_(
    std::vector<float> &vertices,
    std::vector<unsigned int> &indices,
    unsigned int &base,
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

  float dist_sq =
      ((((x0 + x1) / 2.0f) - x2) * (((x0 + x1) / 2.0f) - x2)) +
      ((((y0 + y1) / 2.0f) - y2) * (((y0 + y1) / 2.0f) - y2));
  if (dist_sq > 2.0f) {
    add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x2, y2, a2);
    push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
    add_oval_opaque_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x1, y1, a1);
  } else
    push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
}

void LinedBatch::add_oval_alpha_(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
  check_initialize_alpha_();

  std::vector<float> vertices{};
  std::vector<unsigned int> indices{};
  unsigned int base = alpha_vertices_->size() / floats_per_vertex_;

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

  push_vertex(vertices, indices, base, x0, y0, z, r, g, b, a, cx, cy, angle);
  add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x1, y1, a1);

  push_vertex(vertices, indices, base, x1, y1, z, r, g, b, a, cx, cy, angle);
  add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);

  push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
  add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x3, y3, a3);

  push_vertex(vertices, indices, base, x3, y3, z, r, g, b, a, cx, cy, angle);
  add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x3, y3, a3, x0, y0, a4);

  indices.push_back(65565);
  alpha_indices_->add(indices);
  alpha_vertices_->add(vertices);
}

void LinedBatch::add_oval_alpha_recurse_(
    std::vector<float> &vertices,
    std::vector<unsigned int> &indices,
    unsigned int &base,
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

  float dist_sq =
      ((((x0 + x1) / 2.0f) - x2) * (((x0 + x1) / 2.0f) - x2)) +
      ((((y0 + y1) / 2.0f) - y2) * (((y0 + y1) / 2.0f) - y2));
  if (dist_sq > 2.0f) {
    add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x2, y2, a2);
    push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
    add_oval_alpha_recurse_(vertices, indices, base, x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x1, y1, a1);
  } else
    push_vertex(vertices, indices, base, x2, y2, z, r, g, b, a, cx, cy, angle);
}

} // namespace baphomet::gl
