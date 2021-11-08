#ifndef HADES_WINDOW_HPP
#define HADES_WINDOW_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "hades/gl/context.hpp"
#include "hades/internal/bitmask_enum.hpp"
#include "hades/eventmgr.hpp"

#include <memory>
#include <string>

namespace hades {

enum class WFlags {
    none,
    resizable,
    vsync,
    hidden,
    centered
};

struct WCfg {
    std::string title;
    glm::ivec2 size;
    glm::ivec2 glversion;

    int monitor{0};
    glm::ivec2 position{0, 0};

    WFlags flags{WFlags::none};
};

class WindowMgr;  // forward declare

class Window {
    friend class WindowMgr;

public:
    std::string tag;

    virtual ~Window();

protected:
    WindowMgr *mgr{nullptr};

    std::unique_ptr<gl::Context> ctx;
    std::unique_ptr<EventMgr> events;

    virtual void initialize();
    virtual void update(double dt);
    virtual void draw();

private:
    GLFWwindow *glfw_window_{nullptr};
    WCfg open_cfg_{};

    glm::ivec2 size_{0, 0};
    glm::ivec2 pos_{0, 0};

    void make_current_();

    void open_();
    void initialize_gl_();
};

} // namespace hades

ENABLE_BITMASK_OPERATORS(hades::WFlags)

#endif //HADES_WINDOW_HPP
