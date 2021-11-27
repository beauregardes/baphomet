#include "hades/hades.hpp"

#include <vector>

class SpinningOvals : public hades::Application {
public:
    const int XGAP{10}, YGAP{10};

    std::vector<std::vector<glm::vec2>> sizes;
    std::vector<std::vector<hades::RGB>> colors;
    std::vector<std::vector<float>> rotations;

    double angle{0.0};

    void initialize() override {
        rnd::debug_show_seed();

        sizes = std::vector<std::vector<glm::vec2>>(
            (window->h() / YGAP) + 1,
            std::vector<glm::vec2>((window->w() / XGAP) + 1)
        );
        colors = std::vector<std::vector<hades::RGB>>(
            (window->h() / YGAP) + 1,
            std::vector<hades::RGB>((window->w() / XGAP) + 1)
        );
        rotations = std::vector<std::vector<float>>(
            (window->h() / YGAP) + 1,
            std::vector<float>((window->w() / XGAP) + 1)
        );

        for (int y = 0; y < sizes.size(); ++y)
            for (int x = 0; x < sizes[y].size(); ++x) {
                sizes[y][x] = {rnd::get(XGAP / 2.0f, XGAP * 1.5f), rnd::get(YGAP / 2.0f, YGAP * 1.5f)};
                colors[y][x] = rnd::rgba({0, 255}, {0, 255}, {128, 255}, {64, 64});
                rotations[y][x] = rnd::get(1.0f, 16.0f);
            }
    }

    void update(double dt) override {
        if (input->pressed("escape"))
            shutdown();

        angle += 360.0 * dt;
    }

    void draw() override {
        gfx->clear_color(hades::rgb(0x181818));
        gfx->clear();

        for (int y = 0; y < sizes.size(); ++y)
            for (int x = 0; x < sizes[y].size(); ++x)
                gfx->oval(
                    x * XGAP,
                    y * YGAP,
                    sizes[y][x].x,
                    sizes[y][x].y,
                    colors[y][x],
                    angle / rotations[y][x]
                );
    }
};

int main(int, char *[]) {
    hades::Runner()
        .open<SpinningOvals>({
            .title = "Spinning Ovals",
            .monitor = 1,
            .flags = hades::WFlags::borderless
        })
        .initgl()
        .start();
}


