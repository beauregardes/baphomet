#include "hades/hades.hpp"

const auto vsh_src = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 fColor;

uniform float zMax;
uniform mat4 projection;

void main() {
    fColor = aColor;
    gl_Position = projection * vec4(aPos.xy, -(zMax - aPos.z) / (zMax + 1.0), 1.0);
}
)glsl";

const auto fsh_src = R"glsl(
#version 330 core
in vec4 fColor;

out vec4 FragColor;

void main() {
    FragColor = fColor;
}
)glsl";

class Scratch : public hades::Window {
public:
    std::unique_ptr<gl::Framebuffer> fbo;

    std::unique_ptr<gl::Shader> shader;
    glm::mat4 projection;

    std::unique_ptr<gl::VecBuffer<float>> vertices;
    std::unique_ptr<gl::VertexArray> vao;

    float z_level = 1.0f;

    void initialize() override {
        ctx->enable(gl::Capability::blend);
        ctx->blend_func(gl::BlendFunc::src_alpha, gl::BlendFunc::one_minus_src_alpha);

        ctx->enable(gl::Capability::depth_test);

        fbo = ctx->framebuffer(window_width(), window_height())
            .renderbuffer(gl::RBufFormat::rgba8)
            .renderbuffer(gl::RBufFormat::d32f)
            .check_complete();

        shader = ctx->shader()
            .vert_from_src(vsh_src)
            .frag_from_src(fsh_src)
            .link();

        projection = create_ortho_projection();

        vertices = ctx->vec_buffer<float>(
            gl::BufTarget::array, gl::BufUsage::static_draw, 7);

        vao = ctx->vertex_array();
        vao->attrib_pointer(vertices.get(), {
            {0, 3, gl::AttrType::float_t, false, sizeof(float) * 7, 0},
            {1, 4, gl::AttrType::float_t, false, sizeof(float) * 7, sizeof(float) * 3}
        });
    }

    void update(double dt) override {
        auto r = hades::rand::get<float>(0.5f, 1.0f);
        auto g = hades::rand::get<float>(0.5f, 1.0f);
        auto b = hades::rand::get<float>(0.5f, 1.0f);
        float x = (float)hades::rand::get<int>(0, window_width()) + 0.5f;
        float y = (float)hades::rand::get<int>(0, window_height()) + 0.5f;
        vertices->add({x, y, z_level, r, g, b, 1.0f});
        z_level++;
    }

    void draw() override {
        fbo->push([&]{
            ctx->clear_color(hades::rgb(0x000000));
            ctx->clear(gl::ClearMask::color | gl::ClearMask::depth);

            vertices->sync();

            shader->use();
            shader->uniform_1f("zMax", z_level);
            shader->uniform_mat4f("projection", projection);

            vao->draw_arrays(gl::DrawMode::points, vertices->front() / 7, vertices->size() / 7);
        });
        fbo->copy_to_default_framebuffer();
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>("Scratch", {
       .title = "Scratch",
       .size = {1280, 720},
       .glversion = {4, 5},
       .monitor = 1,
       .flags = hades::WFlags::centered
    });

    e.event_loop();
}