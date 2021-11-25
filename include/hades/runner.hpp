#ifndef HADES_RUNNER_HPP
#define HADES_RUNNER_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

#include "hades/application.hpp"

#include <concepts>

namespace hades {

template<typename T>
concept DerivesHadesApplication = std::is_base_of_v<Application, T>;

class Runner {
public:
    Runner();
    ~Runner();

    template<DerivesHadesApplication T>
    Runner &open(const WCfg &cfg);

    Runner &initgl(glm::ivec2 glversion = {3, 3});

    void start();

private:
    struct {
        glm::ivec2 glversion{};
        WCfg cfg{};
    } open_params_;

    std::unique_ptr<Application> application_;

    void register_glfw_callbacks_();
    static void glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos);
    static void glfw_cursor_enter_callback_(GLFWwindow *window, int entered);
    static void glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods);
    static void glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset);
    static void glfw_window_size_callback_(GLFWwindow *window, int width, int height);
    static void glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos);
    static void glfw_window_focus_callback_(GLFWwindow *window, int focused);
};

template<DerivesHadesApplication T>
Runner &Runner::open(const WCfg &cfg) {
    application_ = std::unique_ptr<Application>(new T());
    open_params_.cfg = cfg;
    return *this;
}

} // namespace hades

#endif //HADES_RUNNER_HPP
