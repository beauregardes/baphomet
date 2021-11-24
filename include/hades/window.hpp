#ifndef HADES_WINDOW_HPP
#define HADES_WINDOW_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "hades/context.hpp"
#include "hades/internal/bitmask_enum.hpp"
#include "hades/util/framecounter.hpp"
#include "hades/util/ticker.hpp"
#include "hades/util/timermgr.hpp"
#include "hades/eventmgr.hpp"

#include <memory>
#include <string>

namespace hades {

enum class WFlags {
    none       = 1 << 0,
    resizable  = 1 << 1,
    vsync      = 1 << 2,
    hidden     = 1 << 3,
    centered   = 1 << 4,
    fullscreen = 1 << 5,
    borderless = 1 << 6
};

struct WCfg {
    std::string title;
    glm::ivec2 size{0, 0};
    glm::ivec2 glversion{3, 3};

    int monitor{0};
    glm::ivec2 position{0, 0};

    WFlags flags{WFlags::none};
};

class WindowMgr;

class Window {
    friend class WindowMgr;

public:
    std::string tag;

    virtual ~Window();

protected:
    WindowMgr *mgr{nullptr};

    std::unique_ptr<Context> ctx{nullptr};
    std::unique_ptr<EventMgr> events{nullptr};
    std::unique_ptr<TimerMgr> timers{nullptr};

    virtual void initialize();
    virtual void update(double dt);
    virtual void draw();

    void window_close();

    void window_set_auto_iconify(bool auto_iconify);
    bool window_auto_iconify();

    void window_set_floating(bool floating);
    bool window_floating();

    void window_set_resizable(bool resizable);
    bool window_resizable();

    void window_set_visible(bool visible);
    bool window_visible();

    void window_set_decorated(bool decorated);
    bool window_decorated();

    void window_set_vsync(bool vsync);
    bool window_vsync();

    void window_set_size(int width, int height);
    glm::ivec2 window_size();
    void window_set_size_limits(int min_width, int min_height, int max_width, int max_height);
    void window_set_aspect_ratio(int numerator, int denominator);

    void window_set_pos(int x, int y);
    glm::ivec2 window_pos();

    glm::mat4 window_projection();

private:
    GladGLContext *ctx_{nullptr};

    std::unique_ptr<gl::Framebuffer> fbo_{nullptr};

    Ticker dt_timer_{};

    GLFWwindow *glfw_window_{nullptr};
    WCfg open_cfg_{};

    struct {
        bool borderless{false};
        bool vsync{false};
    } wm_info_;

    struct {
        FrameCounter frame_counter{};
        std::unique_ptr<hades::CP437> font{nullptr};
    } overlay_;

    void start_frame_();
    void end_frame_();

    void make_current_();

    void open_();

    GLFWmonitor *get_monitor_();
    void open_fullscreen_windows_();
    void open_fullscreen_linux_();
    void open_windowed_();

    void initialize_gl_();
};

} // namespace hades

ENABLE_BITMASK_OPERATORS(hades::WFlags)

#endif //HADES_WINDOW_HPP
