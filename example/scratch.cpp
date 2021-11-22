#include "hades/hades.hpp"

class Scratch : public hades::Window {
public:
    hades::FrameCounter frame_counter{};
    std::unique_ptr<hades::Texture> font_tex{nullptr};

    void initialize() override {
        font_tex = ctx->load_texture("resources/font/1px_6x8.png", true);

        timers->every(0.25, [&]{ fmt::print("FPS: {:.2f}\n", frame_counter.fps()); });
    }

    void update(double dt) override {
        frame_counter.update();

        if (events->pressed("escape"))
            window_close();
    }

    void draw() override {
        ctx->clear(gl::ClearMask::color | gl::ClearMask::depth);

        font_tex->draw(0, 0, font_tex->width() * 2, font_tex->height() * 2);
        font_tex->draw(20, 20, hades::rgb(0xff0000));
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
        .title = "Scratch 1",
        .size = {1280, 720},
        .glversion = {4, 5},
        .monitor = 1,
        .flags = hades::WFlags::centered
    });

    e.event_loop();
}