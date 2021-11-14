#include "hades/window.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"

namespace hades {

Window::~Window() {
    mgr = nullptr;

    ctx.reset(nullptr);
    events.reset(nullptr);
}

void Window::initialize() {}
void Window::update(double dt) {}
void Window::draw() {}

glm::mat4 Window::create_ortho_projection(float z_near, float z_far) {
    return glm::ortho(
        0.0f, static_cast<float>(size_.x),
        static_cast<float>(size_.y), 0.0f,
        z_near, z_far
    );
}

void Window::make_current_() {
    glfwMakeContextCurrent(glfw_window_);
}

void Window::open_() {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, set(open_cfg_.flags, WFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, open_cfg_.glversion.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, open_cfg_.glversion.y);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfw_window_ = glfwCreateWindow(open_cfg_.size.x, open_cfg_.size.y, open_cfg_.title.c_str(), nullptr, nullptr);
    if (!glfw_window_) {
        const char *description;
        int code = glfwGetError(&description);
        spdlog::critical("({}) Failed to create GLFW window:\n* ({}) {}", tag, code, description);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    } else
        spdlog::debug("({}) Created GLFW window", tag);
    size_ = open_cfg_.size;

    int monitor_count;
    auto monitors = glfwGetMonitors(&monitor_count);

    GLFWmonitor *monitor;
    if (open_cfg_.monitor >= monitor_count) {
        spdlog::warn(
            "({}) Monitor {} out of range (only {} monitors available); defaulting to primary",
            tag,
            open_cfg_.monitor,
            monitor_count
        );
        monitor = monitors[0];
    } else
        monitor = monitors[open_cfg_.monitor];

    if (set(open_cfg_.flags, WFlags::centered)) {
        int base_x, base_y, mon_w, mon_h;
        glfwGetMonitorWorkarea(monitor, &base_x, &base_y, &mon_w, &mon_h);
        pos_ = {base_x + (mon_w - size_.x) / 2, base_y + (mon_h - size_.y) / 2};
    } else {
        int base_x, base_y;
        glfwGetMonitorPos(monitor, &base_x, &base_y);
        pos_ = {base_x + open_cfg_.position.x, base_y + open_cfg_.position.y};
    }
    glfwSetWindowPos(glfw_window_, pos_.x, pos_.y);

    events = std::make_unique<EventMgr>(glfw_window_);
    timers = std::make_unique<TimerMgr>();

    if (!set(open_cfg_.flags, WFlags::hidden))
        glfwShowWindow(glfw_window_);
}

void Window::initialize_gl_() {
    make_current_();

    ctx = std::make_unique<gl::Context>(tag, open_cfg_.glversion);

    glfwSwapInterval(set(open_cfg_.flags, WFlags::vsync) ? 1 : 0);
}

} // namespace hades