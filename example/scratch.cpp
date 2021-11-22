#include "hades/hades.hpp"

class Scratch : public hades::Window {
public:
    hades::FrameCounter frame_counter{};

    void initialize() override {
        timers->every(0.25, [&]{
            fmt::print("FPS: {:.2f}\n", frame_counter.fps());
        });
    }

    void update(double dt) override {
        frame_counter.update();

        if (events->pressed("escape"))
            window_close();
    }

    void draw() override {
        ctx->clear(gl::ClearMask::color | gl::ClearMask::depth);

        for (int i = 0; i < 1000; i++)
            ctx->line(
                hades::rand::get<int>(0, window_width() - 1),
                hades::rand::get<int>(0, window_height() - 1),
                hades::rand::get<int>(0, window_width() - 1),
                hades::rand::get<int>(0, window_height() - 1),
                hades::rand::rgb()
            );
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
        .title = "Scratch 1",
        .size = {500, 500},
        .glversion = {4, 5},
        .monitor = 1,
        .position = {100, 100},
//        .flags = hades::WFlags::centered
    });

    e.open<Scratch>({
        .title = "Scratch 2",
        .size = {500, 500},
        .glversion = {4, 5},
        .monitor = 1,
        .position = {700, 100},
//        .flags = hades::WFlags::centered
    });

    e.event_loop();
}