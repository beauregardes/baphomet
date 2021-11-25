#include "hades/runner.hpp"

namespace hades {

Runner::Runner() {
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

Runner::~Runner() {
    glfwTerminate();
}

Runner &Runner::initgl(glm::ivec2 glversion) {
    open_params_.glversion = glversion;
    return *this;
}

void Runner::start() {
    application_->open_(open_params_.cfg, open_params_.glversion);
    register_glfw_callbacks_();
    
    application_->initgl_(open_params_.glversion);

    application_->initialize();

    Ticker t{};
    do {
        t.tick();

        application_->overlay_.frame_counter.update();
        application_->update(t.dt_sec());

        application_->start_frame_();
        application_->draw();
        application_->end_frame_();

        application_->input->update_(t.dt_sec());
        application_->timer->update(t.dt_sec());

        glfwPollEvents();
    } while (!glfwWindowShouldClose(application_->window->glfw_window_));

    GladGLContext *glad_context = application_->ctx_;
    application_.reset(nullptr);
    delete glad_context;
}

void Runner::register_glfw_callbacks_() {
    GLFWwindow *w = application_->window->glfw_window_;
    glfwSetWindowUserPointer(w, this);
    glfwSetKeyCallback(w, glfw_key_callback_);
    glfwSetCursorPosCallback(w, glfw_cursor_position_callback_);
    glfwSetCursorEnterCallback(w, glfw_cursor_enter_callback_);
    glfwSetMouseButtonCallback(w, glfw_mouse_button_callback_);
    glfwSetScrollCallback(w, glfw_scroll_callback_);
    glfwSetWindowSizeCallback(w, glfw_window_size_callback_);
    glfwSetWindowPosCallback(w, glfw_window_pos_callback_);
    glfwSetWindowFocusCallback(w, glfw_window_focus_callback_);
    w = nullptr;
}

void Runner::glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    runner->application_->input->glfw_key_event_(key, scancode, action, mods);
}

void Runner::glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    runner->application_->input->glfw_cursor_position_event_(xpos, ypos);
}

void Runner::glfw_cursor_enter_callback_(GLFWwindow *window, int entered) {
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    runner->application_->input->glfw_cursor_enter_event_(entered);
}

void Runner::glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    runner->application_->input->glfw_mouse_button_event_(button, action, mods);
}

void Runner::glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    runner->application_->input->glfw_scroll_event_(xoffset, yoffset);
}

void Runner::glfw_window_size_callback_(GLFWwindow *window, int width, int height) {}

void Runner::glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos) {}

void Runner::glfw_window_focus_callback_(GLFWwindow *window, int focused) {
#if defined(_WIN32) || defined(__CYGWIN__)
    auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
    if (runner->application_->window->wm_info_.borderless)
        runner->application_->window->set_floating(focused == 1);
#endif
}

} // namespace hades
