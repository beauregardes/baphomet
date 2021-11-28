#include "gl/batching/pixel_batch.hpp"

namespace gl {

PixelBatch::PixelBatch(GladGLContext *ctx) : Batch(ctx, 7) {
  shader_ = ShaderBuilder(ctx_, "PixelBatch")
            .vert_from_src(R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;

out vec4 out_color;

uniform float z_max;
uniform mat4 projection;

void main() {
  float z = -(z_max - in_pos.z) / (z_max + 1.0);
  gl_Position = projection * vec4(in_pos.xy, z, 1.0);
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

void PixelBatch::add(
  float x, float y,
  float z,
  float r, float g, float b, float a
) {
  if (a < 1.0f)
    add_alpha_(x, y, z, r, g, b, a);
  else
    add_opaque_(x, y, z, r, g, b, a);
}

void PixelBatch::draw_opaque(float z_max, glm::mat4 projection) {
  if (!empty_opaque()) {
    opaque_vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    opaque_vao_->draw_arrays(
      DrawMode::points,
      opaque_vertices_->front() / floats_per_vertex_,
      opaque_vertices_->size() / floats_per_vertex_
    );
  }
}

void PixelBatch::draw_alpha(float z_max, glm::mat4 projection) {
  if (!empty_alpha()) {
    alpha_vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    alpha_vao_->draw_arrays(
      DrawMode::points,
      alpha_vertices_->front() / floats_per_vertex_,
      alpha_vertices_->size() / floats_per_vertex_
    );
  }
}

void PixelBatch::add_opaque_(
  float x, float y,
  float z,
  float r, float g, float b, float a
) {
  if (!opaque_vertices_) {
    opaque_vertices_ = std::make_unique<VecBuffer<float>>(
      ctx_, floats_per_vertex_, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    opaque_vao_ = std::make_unique<VertexArray>(ctx_);
    opaque_vao_->attrib_pointer(opaque_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 7, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 7, sizeof(float) * 3}
    });
  }

  opaque_vertices_->add({x, y, z, r, g, b, a});
}

void PixelBatch::add_alpha_(
  float x, float y,
  float z,
  float r, float g, float b, float a
) {
  if (!alpha_vertices_) {
    alpha_vertices_ = std::make_unique<VecBuffer<float>>(
      ctx_, floats_per_vertex_, false, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    alpha_vao_ = std::make_unique<VertexArray>(ctx_);
    alpha_vao_->attrib_pointer(alpha_vertices_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 7, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 7, sizeof(float) * 3}
    });
  }

  alpha_vertices_->add({x, y, z, r, g, b, a});
}

} // namespace gl