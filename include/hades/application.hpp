#ifndef HADES_APPLICATION_HPP
#define HADES_APPLICATION_HPP

#include "gl/framebuffer.hpp"

#include "hades/gfxmgr.hpp"
#include "hades/inputmgr.hpp"
#include "hades/window.hpp"

#include "hades/util/timermgr.hpp"

namespace hades {

class Application {
    friend class Runner;

public:
    virtual ~Application() = default;

protected:
    std::unique_ptr<Window> window{nullptr};
    std::unique_ptr<InputMgr> input{nullptr};
    std::unique_ptr<GfxMgr> gfx{nullptr};

    std::unique_ptr<TimerMgr> timer{nullptr};

    virtual void initialize();
    virtual void update(double dt);
    virtual void draw();

    void shutdown();

private:
    GladGLContext *ctx_{nullptr};

    std::unique_ptr<gl::Framebuffer> fbo_{nullptr};

    struct {
        FrameCounter frame_counter{};
        std::unique_ptr<hades::CP437> font{nullptr};
    } overlay_{};

    void start_frame_();
    void end_frame_();

    void draw_overlay_();

    /******************
     * INITIALIZATION *
     ******************/

    void open_(const WCfg &cfg, glm::ivec2 glversion);
    void initgl_(glm::ivec2 glversion);
};

} // namespace hades

#endif //HADES_APPLICATION_HPP
