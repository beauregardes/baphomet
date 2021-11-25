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
    glm::ivec2 position{0, 0};

    int monitor{0};

    WFlags flags{WFlags::none};
};

class Window {
    friend class Application;
    friend class Runner;

public:
    void set_auto_iconify(bool auto_iconify);
    bool auto_iconify();

    void set_floating(bool floating);
    bool floating();

    void set_resizable(bool resizable);
    bool resizable();

    void set_visible(bool visible);
    bool visible();

    void set_decorated(bool decorated);
    bool decorated();

    void set_vsync(bool vsync);
    bool vsync();

    void set_size(int width, int height);
    glm::ivec2 size();
    int w();
    int h();

    void set_size_limits(int min_width, int min_height, int max_width, int max_height);
    void set_aspect_ratio(int numerator, int denominator);

    void set_pos(int x, int y);
    glm::ivec2 pos();
    int x();
    int y();

    glm::mat4 projection();

private:
    GLFWwindow *glfw_window_{nullptr};

    struct {
        bool borderless{false};
        bool vsync{false};
    } wm_info_;

    void open_(const WCfg &cfg, glm::ivec2 glversion);
    void close_();

    GLFWmonitor *get_monitor_(const WCfg &cfg);
    void open_fullscreen_windows_(const WCfg &cfg);
    void open_fullscreen_linux_(const WCfg &cfg);
    void open_windowed_(const WCfg &cfg);
};

} // namespace hades

ENABLE_BITMASK_OPERATORS(hades::WFlags)

#endif //HADES_WINDOW_HPP
