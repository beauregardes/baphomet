#include "hades/hades.hpp"

class Scratch : public hades::Window {
public:
    void initialize() override {
    }

    void update(double dt) override {
        if (events->pressed("escape"))
            window_close();

        if (events->pressed("1"))
            window_set_vsync(!window_vsync());
    }

    void draw() override {
        ctx->clear_color(hades::rgb(0x101020));
        ctx->clear();

        auto size = window_size();

        ctx->oval(size.x / 2 - 75, size.y / 2 - 45, 100, 60, hades::rgba(0xff8080ff));
        ctx->oval(size.x / 2 - 25, size.y / 2 - 15, 100, 60, hades::rgba(0x80ff80ff));

        ctx->oval(events->mouse.x, events->mouse.y, 60, 60, hades::rgba(0xff80ff80));

        ctx->oval(size.x / 2 + 25, size.y / 2 + 15, 100, 60, hades::rgba(0x8080ffff));
        ctx->oval(size.x / 2 + 75, size.y / 2 + 45, 100, 60, hades::rgba(0xffff80ff));
    }
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
        .title = "Scratch",
        .size = {500, 500},
        .flags = hades::WFlags::centered,
    });

    e.event_loop();
}


