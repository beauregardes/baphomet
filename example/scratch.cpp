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
        ctx->clear_color(hades::rgb(0x404040));
        ctx->clear();

        ctx->oval(150, 75, 100, 50, hades::rgba(0xff808080));
        ctx->oval(200, 100, 100, 50, hades::rgba(0x80ff8080));
        ctx->oval(250, 125, 100, 50, hades::rgba(0x8080ff80));

        ctx->oval(
            events->mouse.x, events->mouse.y,
            100, 50,
            hades::rgba(0xff80ff80)
        );
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