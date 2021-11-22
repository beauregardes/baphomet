#include "gl/batching/rect_batch.hpp"

namespace gl {

RectBatch::RectBatch(GladGLContext *ctx) : Batch(ctx) {
    shader_ = ShaderBuilder(ctx_)
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

    vertices_ = std::make_unique<VecBuffer<float>>(
        ctx_, 60, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    vao_ = std::make_unique<VertexArray>(ctx_);
    vao_->attrib_pointer(vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
        {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
}

void RectBatch::add(
    float x, float y,
    float w, float h,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
    vertices_->add({
        x,     y,     z, r, g, b, a, cx, cy, angle,
        x + w, y,     z, r, g, b, a, cx, cy, angle,
        x + w, y + h, z, r, g, b, a, cx, cy, angle,
        x,     y,     z, r, g, b, a, cx, cy, angle,
        x + w, y + h, z, r, g, b, a, cx, cy, angle,
        x,     y + h, z, r, g, b, a, cx, cy, angle
    });
}

void RectBatch::draw(float z_max, glm::mat4 projection) {
    vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    vao_->draw_arrays(
        DrawMode::triangles,
        vertices_->front() / 10,
        vertices_->size() / 10
    );
}

} // namespace gl