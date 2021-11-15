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

class Histogram {
public:
    Histogram() = default;

    void add(double v) {
        data_.push_back(v);
    }

    void print(int buckets, int max_bar_length) {
        double min = std::log2(*std::min_element(data_.begin(), data_.end()));
        double max = std::log2(*std::max_element(data_.begin(), data_.end()));

        std::vector<int> bucket_counts(buckets, 0);
        for (double v : data_) {
            int b = (int)(normalize(std::log2(v), min, max, 0.0, (double)buckets - 1));
            bucket_counts[b]++;
        }

        double max_bucket_count = *std::max_element(bucket_counts.begin(), bucket_counts.end());

        fmt::print("log2 scaling\n");
        fmt::print("------------\n");
        for (int i = 0; i < bucket_counts.size(); ++i) {
            double b_min = min + ((double)i / (double)buckets) * (max - min);
            double b_max = min + ((double)(i + 1) / (double)buckets) * (max - min);
            double b_count = (double)bucket_counts[i];
            int b_length = (int)((b_count / max_bucket_count) * max_bar_length);

            fmt::print("({:10.5f} - {:10.5f}) | ", b_min, b_max);
            for (int i = 0; i < b_length; ++i)
                fmt::print("#");
            for (int i = 0; i < (max_bar_length - b_length); ++i)
                fmt::print(" ");
            fmt::print(" {}\n", b_count);
        }
        fmt::print("------------\n");
        fmt::print("Total frames: {}\n", data_.size());
    }

private:
    std::vector<double> data_{};

    double normalize(double x, double x_min, double x_max, double a, double b) {
        return (b - a) * ((x - x_min) / (x_max - x_min)) + a;
    }
};

class Scratch : public hades::Window {
public:
    std::unique_ptr<gl::Framebuffer> fbo;

    std::unique_ptr<gl::Shader> shader;
    glm::mat4 projection;

    std::unique_ptr<gl::VecBuffer<float>> vertices;
    std::unique_ptr<gl::StaticBuffer<float>> colors;
    std::unique_ptr<gl::VertexArray> vao;

    float z_level = 1.0f;

    Histogram fps_histo{};
    bool first_dt = true;

    ~Scratch() override { mgr->shutdown(); }

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

        projection = create_ortho_projection(0.0f, 1.0f);

        vertices = ctx->vec_buffer<float>(
            gl::BufTarget::array, gl::BufUsage::static_draw, 7);

        vao = ctx->vertex_array();
        vao->attrib_pointer(vertices.get(), {
            {0, 3, gl::AttrType::float_t, false, sizeof(float) * 7, 0},
            {1, 4, gl::AttrType::float_t, false, sizeof(float) * 7, sizeof(float) * 3}
        });

        timers->after(60.0, [&]{
            fps_histo.print(20, 32);
            mgr->shutdown();
        });
    }

    void update(double dt) override {
        if (first_dt)
            first_dt = false;
        else
            fps_histo.add(dt);

        auto r = hades::rand::get<float>();
        auto g = hades::rand::get<float>();
        auto b = hades::rand::get<float>();
        float w = (float)hades::rand::get<int>(2, 5);
        float h = (float)hades::rand::get<int>(2, 5);
        float x = (float)hades::rand::get<int>(0, window_width() - w);
        float y = (float)hades::rand::get<int>(0, window_height() - h);
        vertices->add({
            x,     y,     z_level,   r, g, b, 1.0f,
            x + w, y,     z_level,   r, g, b, 1.0f,
            x + w, y + h, z_level,   r, g, b, 1.0f,
            x,     y,     z_level,   r, g, b, 1.0f,
            x + w, y + h, z_level,   r, g, b, 1.0f,
            x,     y + h, z_level,   r, g, b, 1.0f
        });
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

            vao->draw_arrays(gl::DrawMode::triangles, vertices->front() / 7, vertices->size() / 7);
        });
        fbo->copy_to_default_framebuffer();
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
       .title = "Scratch",
       .size = {1280, 720},
       .glversion = {4, 5},
       .flags = hades::WFlags::centered
    });

    e.event_loop();
}