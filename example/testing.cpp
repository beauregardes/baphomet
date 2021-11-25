#include "hades/hades.hpp"

class Scratch : public hades::Application {
public:
    void initialize() override {
    }

    void update(double dt) override {
        if (events->pressed("escape"))
            shutdown();

        if (events->pressed("1"))
            window->set_vsync(!window->vsync());
    }

    void draw() override {
        ctx->clear_color(hades::rgb(0x101030));
        ctx->clear();

        ctx->oval(window->w() / 2 - 75, window->h() / 2 - 45, 100, 60, hades::rgba(0xff8080ff));
        ctx->oval(window->w() / 2 - 25, window->h() / 2 - 15, 100, 60, hades::rgba(0x80ff80ff));

        ctx->oval(events->mouse.x, events->mouse.y, 60, 60, hades::rgba(0xff80ff80));

        ctx->oval(window->w() / 2 + 25, window->h() / 2 + 15, 100, 60, hades::rgba(0x8080ffff));
        ctx->oval(window->w() / 2 + 75, window->h() / 2 + 45, 100, 60, hades::rgba(0xffff80ff));
    }
};

int main(int, char *[]) {
    hades::Runner()
        .open<Scratch>({
            .title = "Scratch",
            .size = {500, 500},
            .monitor = 1,
            .flags = hades::WFlags::borderless
        })
        .initgl()
        .start();
}


