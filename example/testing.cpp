#include "hades/hades.hpp"

class Testing : public hades::Application {
public:
    std::vector<glm::vec4> rects{};
    std::vector<hades::RGB> colors{};

    float rad{15.0f};

    void initialize() override {
        rnd::debug_show_seed();
    }

    void update(double dt) override {
        if (input->pressed("escape"))
            shutdown();

        if (input->pressed("mb_left")) {
            rects.emplace_back(input->mouse.x, input->mouse.y, rad, rad);
            colors.emplace_back(rnd::rgb());
        }

        if (input->pressed("r")) {
            rects.clear();
            colors.clear();
        }

        if (input->mouse.sy > 0)
            rad += 1.0f;
        else if (input->mouse.sy < 0)
            rad -= 1.0f;
    }

    void draw() override {
        gfx->clear_color(hades::rgb(0x181818));
        gfx->clear();

        for (std::size_t i = 0; i < rects.size(); ++i)
            gfx->oval(rects[i].x, rects[i].y, rects[i].z, rects[i].w, colors[i]);
    }
};

int main(int, char *[]) {
    hades::Runner()
        .open<Testing>({
            .title = "Testing",
            .monitor = 1,
            .flags = hades::WFlags::borderless
        })
        .initgl()
        .start();
}


