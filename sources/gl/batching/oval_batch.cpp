#include "gl/batching/oval_batch.hpp"

namespace gl {

OvalBatch::OvalBatch(GladGLContext *ctx) : Batch(ctx) {
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
        ctx_, 30, true, gl::BufTarget::array, gl::BufUsage::dynamic_draw);

    vao_ = std::make_unique<VertexArray>(ctx_);
    vao_->attrib_pointer(vertices_.get(), {
        {0, 3, gl::AttrType::float_t, false, sizeof(float) * 10, 0},
        {1, 4, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 3},
        {2, 3, gl::AttrType::float_t, false, sizeof(float) * 10, sizeof(float) * 7}
    });
}

void OvalBatch::add(
    float x, float y,
    float x_radius, float y_radius,
    float z,
    float r, float g, float b, float a,
    float cx, float cy, float angle
) {
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

    vertices_->add({
        x0, y0, z, r, g, b, a, cx, cy, angle,
        x,  y,  z, r, g, b, a, cx, cy, angle,
        x1, y1, z, r, g, b, a, cx, cy, angle,

        x1, y1, z, r, g, b, a, cx, cy, angle,
        x,  y,  z, r, g, b, a, cx, cy, angle,
        x2, y2, z, r, g, b, a, cx, cy, angle,

        x2, y2, z, r, g, b, a, cx, cy, angle,
        x,  y,  z, r, g, b, a, cx, cy, angle,
        x3, y3, z, r, g, b, a, cx, cy, angle,

        x3, y3, z, r, g, b, a, cx, cy, angle,
        x,  y,  z, r, g, b, a, cx, cy, angle,
        x0, y0, z, r, g, b, a, cx, cy, angle
    });

    add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x1, y1, a1);
    add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
    add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x2, y2, a2, x3, y3, a3);
    add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x3, y3, a3, x0, y0, a4);
}

void OvalBatch::draw(float z_max, glm::mat4 projection) {
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

void OvalBatch::add_(
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

    vertices_->add({
        x0, y0, z, r, g, b, a, cx, cy, angle,
        x2, y2, z, r, g, b, a, cx, cy, angle,
        x1, y1, z, r, g, b, a, cx, cy, angle
    });

    float dist_sq =
        ((((x0 + x1) / 2.0f) - x2) * (((x0 + x1) / 2.0f) - x2)) +
        ((((y0 + y1) / 2.0f) - y2) * (((y0 + y1) / 2.0f) - y2));
    if (dist_sq > 2.0f) {
        add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x0, y0, a0, x2, y2, a2);
        add_(x, y, x_radius, y_radius, z, r, g, b, a, cx, cy, angle, x1, y1, a1, x2, y2, a2);
    }

}

} // namespace gl