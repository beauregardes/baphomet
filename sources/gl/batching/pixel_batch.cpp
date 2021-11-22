#include "gl/batching/pixel_batch.hpp"

namespace gl {

PixelBatch::PixelBatch(GladGLContext *ctx) : Batch(ctx) {
    shader_ = ShaderBuilder(ctx_)
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

    vertices_ = std::make_unique<VecBuffer<float>>(
        ctx_, 7, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    vao_ = std::make_unique<VertexArray>(ctx_);
    vao_->attrib_pointer(vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 7, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 7, sizeof(float) * 3}
    });
}

void PixelBatch::add(float x, float y, float z, float r, float g, float b, float a) {
    vertices_->add({x, y, z, r, g, b, a});
}

void PixelBatch::draw(float z_max, glm::mat4 projection) {
    vertices_->sync();

    shader_->use();
    shader_->uniform_1f("z_max", z_max);
    shader_->uniform_mat4f("projection", projection);

    vao_->draw_arrays(
        DrawMode::points,
        vertices_->front() / 7,
        vertices_->size() / 7
    );
}

} // namespace gl