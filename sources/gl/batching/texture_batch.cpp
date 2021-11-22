#include "gl/batching/texture_batch.hpp"

namespace gl {

TextureBatch::TextureBatch(GladGLContext *ctx, const std::unique_ptr<gl::TextureUnit> &texture_unit)
    : Batch(ctx), texture_unit_(texture_unit) {

    shader_ = ShaderBuilder(ctx_)
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

    vertices_ = std::make_unique<VecBuffer<float>>(
        ctx_, 72, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    vao_ = std::make_unique<VertexArray>(ctx_);
    vao_->attrib_pointer(vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 12, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 3},
        {2, 2, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 7},
        {3, 3, gl::AttrType::float_t, false, sizeof(float) * 12, sizeof(float) * 9}
    });

    x_px_unit_ = 1.0f / texture_unit_->width();
    y_px_unit_ = 1.0f / texture_unit_->height();
}

void TextureBatch::add(
    float x, float y,
    float w, float h,
    float tx, float ty,
    float tw, float th,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
    vertices_->add({
        x,     y,     z, r, g, b, a, x_px_unit_ * tx,        y_px_unit_ * ty,        cx, cy, angle,
        x + w, y,     z, r, g, b, a, x_px_unit_ * (tx + tw), y_px_unit_ * ty,        cx, cy, angle,
        x + w, y + h, z, r, g, b, a, x_px_unit_ * (tx + tw), y_px_unit_ * (ty + th), cx, cy, angle,
        x,     y,     z, r, g, b, a, x_px_unit_ * tx,        y_px_unit_ * ty,        cx, cy, angle,
        x + w, y + h, z, r, g, b, a, x_px_unit_ * (tx + tw), y_px_unit_ * (ty + th), cx, cy, angle,
        x,     y + h, z, r, g, b, a, x_px_unit_ * tx,        y_px_unit_ * (ty + th), cx, cy, angle
    });
}

void TextureBatch::draw(float z_max, glm::mat4 projection) {
    vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);
    texture_unit_->bind();

    vao_->draw_arrays(
        DrawMode::triangles,
        vertices_->front() / 10,
        vertices_->size() / 10
    );
}

} // namespace gl