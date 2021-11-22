#ifndef HADES_WINDOWMGR_HPP
#define HADES_WINDOWMGR_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/gl.h"

#include "hades/util/random.hpp"
#include "hades/window.hpp"

#include <concepts>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <queue>
#include <vector>
#include <unordered_map>

namespace hades {

template<typename T>
concept DerivesHadesWindow = std::is_base_of_v<Window, T>;

class WindowMgr {
public:
    WindowMgr();

    ~WindowMgr();

    void event_loop();

    template<DerivesHadesWindow T>
    void open(const std::string &tag, const WCfg &cfg);

    template<DerivesHadesWindow T>
    void open(const WCfg &cfg);

    void shutdown();

private:
    bool shutdown_{false};

    std::queue<std::unique_ptr<Window>> create_queue_{};
    std::queue<std::unique_ptr<Window>> initialize_queue_{};
    std::unordered_map<GLFWwindow *, std::unique_ptr<Window>> windows_{};
    std::queue<std::pair<std::string, GLFWwindow *>> destroy_queue_{};

    void push_destroy_window_(const std::string &tag, GLFWwindow *handle, GladGLContext *ctx);

    static void glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos);
    static void glfw_cursor_enter_callback_(GLFWwindow *window, int entered);
    static void glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods);
    static void glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset);

    static void glfw_window_size_callback_(GLFWwindow *window, int width, int height);
    static void glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos);
};

template<DerivesHadesWindow T>
void WindowMgr::open(const std::string &tag, const WCfg &cfg) {
    auto w = std::make_unique<T>();
    w->mgr = this;
    w->tag = tag;
    w->open_cfg_ = cfg;
    create_queue_.push(std::move(w));

    spdlog::debug("Open requested, pushed to create_queue_ ({})", tag);
}

template<DerivesHadesWindow T>
void WindowMgr::open(const WCfg &cfg) {
    open<T>(hades::rand::base58(11), cfg);
}

} // namespace hades

#endif //HADES_WINDOWMGR_HPP
