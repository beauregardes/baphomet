#include "hades/hades.hpp"

class Scratch : public hades::Window {
public:
    ~Scratch() override { mgr->shutdown(); }

    void initialize() override {
        clear_color_ = hades::lch("red");
    }

    void update(double dt) override {
        fps_avg_.update(1.0 / dt);
        if (fps_show_.tick()) {
            fps_avg_.alpha = 2.0 / (1.0 + (1.0 / dt));
            fmt::print("{:.2f} fps\n", fps_avg_.value());
        }

        clear_color_.h = std::fmod(clear_color_.h + (30.0 * dt), 360.0);
    }

    void draw() override {
        ctx->clear_color(clear_color_.to_rgb());
        ctx->clear(gl::ClearBit::color | gl::ClearBit::depth);
    }

private:
    hades::Ticker fps_show_{1.0};
    hades::EMA fps_avg_{1.0};

    hades::CIELCh clear_color_;
};

int main(int, char *[]) {
    auto e = hades::WindowMgr();

    e.open<Scratch>({
       .title = "Scratch",
       .size = {600, 480},
       .glversion = {4, 5},
       .flags = hades::WFlags::centered
    });

    e.event_loop();
}