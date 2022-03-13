#include "baphomet/gfx/render_target.hpp"

namespace baphomet {

RenderTarget::RenderTarget(
    const std::string &tag,
    std::uint64_t weight,
    float x, float y, float w, float h
) : tag_(tag), weight_(weight), x_(x), y_(y), w_(w), h_(h) {
  fbo_ = gl::FramebufferBuilder(w, h)
      .texture("color", gl::TexFormat::rgba8)
      .renderbuffer(gl::RBufFormat::d32f)
      .check_complete();
  projection_ = glm::ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f);

  batches_ = std::make_unique<BatchSet>();

  shader_ = gl::ShaderBuilder("RenderTarget")
      .vert_from_src(R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_trans;

out vec4 out_color;
out vec2 out_tex_coords;

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
  out_tex_coords = in_tex_coords;
}
    )glsl")
      .frag_from_src(R"glsl(
#version 330 core
in vec4 out_color;
in vec2 out_tex_coords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
  FragColor = out_color * texture(tex, out_tex_coords);
}
    )glsl")
      .link();

  vbo_ = std::make_unique<gl::VecBuffer<float>>(10 * 6, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);
  vbo_->add({
      x,     y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,     y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,     y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f
  });

  vao_ = std::make_unique<gl::VertexArray>();
  vao_->attrib_pointer(vbo_.get(), {
      {0, 3, gl::AttrType::float_t, false, sizeof(float) * 12, 0},
      {1, 4, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 3},
      {2, 2, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 7},
      {3, 3, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 9}
  });
}

float RenderTarget::x() const {
  return x_;
}

float RenderTarget::y() const {
  return y_;
}

float RenderTarget::w() const {
  return w_;
}

float RenderTarget::h() const {
  return h_;
}

void RenderTarget::resize(float x, float y, float w, float h) {
  x_ = x;
  y_ = y;
  w_ = w;
  h_ = h;

  fbo_ = gl::FramebufferBuilder(w, h)
      .texture("color", gl::TexFormat::rgba8)
      .renderbuffer(gl::RBufFormat::d32f)
      .check_complete();
  projection_ = glm::ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f);

  vbo_->clear();
  vbo_->add({
      x,     y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,     y,     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w, y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,     y + h, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f
  });
}

void RenderTarget::resize(float w, float h) {
  resize(x_, y_, w, h);
}

void RenderTarget::draw_(glm::mat4 window_projection) {
  vbo_->sync();

  shader_->use();
  shader_->uniform_1f("z_max", 2.0f);
  shader_->uniform_mat4f("projection", window_projection);
  fbo_->use_texture("color");

  vao_->draw_arrays(
      gl::DrawMode::triangles,
      vbo_->front() / 12,
      vbo_->size() / 12
  );
}

} // namespace baphomet