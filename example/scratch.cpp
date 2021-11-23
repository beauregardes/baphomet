#include "hades/hades.hpp"

class Scratch : public hades::Window {
public:
    void initialize() override {
    }

    void update(double dt) override {
        if (events->pressed("escape"))
            window_close();
    }

    void draw() override {
        ctx->clear_color(hades::rgb(0x101010));
        ctx->clear();

        ctx->oval(window_width() / 2 - 75, window_height() / 2 - 45, 100, 60, hades::rgba(0xff8080ff));
        ctx->oval(window_width() / 2 - 25, window_height() / 2 - 15, 100, 60, hades::rgba(0x80ff80ff));

        ctx->oval(
            events->mouse.x, events->mouse.y,
            60, 60,
            hades::rgba(0xff80ff80)
        );

        ctx->oval(window_width() / 2 + 25, window_height() / 2 + 15, 100, 60, hades::rgba(0x8080ffff));
        ctx->oval(window_width() / 2 + 75, window_height() / 2 + 45, 100, 60, hades::rgba(0xffff80ff));
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
        .title = "Scratch 1",
        .size = {500, 500},
        .glversion = {4, 5},
        .monitor = 1,
        .flags = hades::WFlags::centered
    });

    e.event_loop();
}


