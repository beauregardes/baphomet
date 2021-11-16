#include "hades/windowmgr.hpp"

#include "spdlog/spdlog.h"

#include "hades/util/ticker.hpp"

namespace hades {

WindowMgr::WindowMgr() {
    spdlog::set_level(spdlog::level::debug);

    if (!glfwInit()) {
        const char *description;
        int code = glfwGetError(&description);

        spdlog::critical(
            "Failed to initialize GLFW:\n* ({}) {}",
            code, description
        );
        std::exit(EXIT_FAILURE);
    } else
        spdlog::debug("Initialized GLFW");
}

WindowMgr::~WindowMgr() {
    glfwTerminate();
    spdlog::debug("Terminated GLFW");
}

void WindowMgr::event_loop() {
    auto render_thread = std::thread([&]{
        spdlog::debug("Started render thread");

        do {
            while (!initialize_queue_.empty()) {
                auto w = std::move(initialize_queue_.front());

                w->initialize_gl_();
                w->initialize();

                std::string tag = w->tag;
                GLFWwindow *handle = w->glfw_window_;
                windows_[handle] = std::move(w);
                spdlog::debug("({}) Initialized, added to windows_", tag);

                initialize_queue_.pop();
            }

            for (auto it = windows_.begin(); it != windows_.end();) {
                auto &w = it->second;
                w->make_current_();

                w->dt_timer_.tick();
                w->update(w->dt_timer_.dt_sec());
                w->events->update_(w->dt_timer_.dt_sec());
                w->timers->update(w->dt_timer_.dt_sec());

                w->draw();

                glfwSwapBuffers(w->glfw_window_);

                if (glfwWindowShouldClose(w->glfw_window_)) {
                    std::string tag = w->tag;
                    GLFWwindow *glfw_window = w->glfw_window_;

                    it = windows_.erase(it);

                    destroy_queue_.emplace(tag, glfw_window);
                    spdlog::debug("({}) Closed, pushed to destroy_queue_", tag);
                    glfwPostEmptyEvent();
                } else
                    it++;
            }

            if (create_queue_.empty() && initialize_queue_.empty() && windows_.empty()) {
                spdlog::debug("Shutting down WindowMgr");
                shutdown();
                glfwPostEmptyEvent();
            }
        } while (!shutdown_);

        for (auto it = windows_.begin(); it != windows_.end();) {
            auto &w = it->second;
            std::string tag = w->tag;
            GLFWwindow *glfw_window = w->glfw_window_;

            glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
            it = windows_.erase(it);

            destroy_queue_.emplace(tag, glfw_window);
            spdlog::debug("({}) Force closing, pushed to destroy_queue_", tag);
        }
        glfwPostEmptyEvent();

        spdlog::debug("Render thread finished");
    });

    spdlog::debug("Started event loop");

    do {
        while (!create_queue_.empty()) {
            auto w = std::move(create_queue_.front());

            w->open_();
            glfwSetWindowUserPointer(w->glfw_window_, this);
            glfwSetKeyCallback(w->glfw_window_, glfw_key_callback_);
            glfwSetCursorPosCallback(w->glfw_window_, glfw_cursor_position_callback_);
            glfwSetCursorEnterCallback(w->glfw_window_, glfw_cursor_enter_callback_);
            glfwSetMouseButtonCallback(w->glfw_window_, glfw_mouse_button_callback_);
            glfwSetScrollCallback(w->glfw_window_, glfw_scroll_callback_);
            glfwSetWindowSizeCallback(w->glfw_window_, glfw_window_size_callback_);
            glfwSetWindowPosCallback(w->glfw_window_, glfw_window_pos_callback_);

            std::string tag = w->tag;
            initialize_queue_.push(std::move(w));
            spdlog::debug("({}) Opened, pushed to initialize_queue_", tag);

            create_queue_.pop();
        }

        glfwWaitEventsTimeout(1.0);

        while (!destroy_queue_.empty()) {
            auto p = destroy_queue_.front();

            glfwDestroyWindow(p.second);
            spdlog::debug("({}) Destroyed GLFW window", p.first);

            destroy_queue_.pop();
        }
    } while (!shutdown_ || !windows_.empty() || !destroy_queue_.empty());

    spdlog::debug("Event loop finished");

    render_thread.join();
}

void WindowMgr::shutdown() {
    shutdown_ = true;
}

void WindowMgr::glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->events->glfw_key_event_(key, scancode, action, mods);
}

void WindowMgr::glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->events->glfw_cursor_position_event_(xpos, ypos);
}

void WindowMgr::glfw_cursor_enter_callback_(GLFWwindow *window, int entered) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->events->glfw_cursor_enter_event_(entered);
}

void WindowMgr::glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->events->glfw_mouse_button_event_(button, action, mods);
}

void WindowMgr::glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->events->glfw_scroll_event_(xoffset, yoffset);
}

void WindowMgr::glfw_window_size_callback_(GLFWwindow *window, int width, int height) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->size_ = glm::ivec2(width, height);
}

void WindowMgr::glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos) {
    auto e = reinterpret_cast<WindowMgr *>(glfwGetWindowUserPointer(window));
    auto it = e->windows_.find(window);
    if (it != e->windows_.end())
        it->second->pos_ = glm::ivec2(xpos, ypos);
}

} // namespace hades