#include "goat/engine.hpp"
#include "glbinding/glbinding.h"
#include "spdlog/spdlog.h"

namespace goat {

Engine::Engine() = default;

void Engine::init_gfx_backend_(const WCfg &cfg) {
  switch (cfg.gfx_backend) {
  case WBackend::gl:
    init_gl_(cfg);
    break;

  default:
    spdlog::critical("Backend NYI");
    std::exit(EXIT_FAILURE);
  }
}

void Engine::init_gl_(const WCfg &cfg) {
  application_->window->make_context_current();
  glbinding::initialize(glfwGetProcAddress);
}

void Engine::register_glfw_callbacks_() {
  auto window = application_->window->glfw_handle();

  glfwSetKeyCallback(window, key_callback_);
  glfwSetCursorPosCallback(window, cursor_position_callback_);
  glfwSetCursorEnterCallback(window, cursor_enter_callback_);
  glfwSetMouseButtonCallback(window, mouse_button_callback_);
  glfwSetScrollCallback(window, scroll_callback_);
  glfwSetJoystickCallback(joystick_callback_);
  glfwSetDropCallback(window, drop_callback_);

  glfwSetWindowCloseCallback(window, window_close_callback_);
  glfwSetWindowSizeCallback(window, window_size_callback_);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_);
  glfwSetWindowContentScaleCallback(window, window_content_scale_callback_);
  glfwSetWindowPosCallback(window, window_pos_callback_);
  glfwSetWindowIconifyCallback(window, window_iconify_callback_);
  glfwSetWindowMaximizeCallback(window, window_maximize_callback_);
  glfwSetWindowFocusCallback(window, window_focus_callback_);
  glfwSetWindowRefreshCallback(window, window_refresh_callback_);

  glfwSetMonitorCallback(monitor_callback_);
}

void Engine::run_() {
  application_->initialize();

  do {
    application_->update(0.0);
    application_->draw();

    application_->window->swap_buffers();

    glfwPollEvents();
  } while (!application_->window->should_close());
}

/******************
 * GLFW CALLBACKS *
 ******************/

void Engine::key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
  spdlog::debug("Key callback {} {} {} {}", key, scancode, action, mods);
}

void Engine::cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {}

void Engine::cursor_enter_callback_(GLFWwindow *window, int entered) {}

void Engine::mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {}

void Engine::scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {}

void Engine::joystick_callback_(int jid, int event) {}

void Engine::drop_callback_(GLFWwindow *window, int count, const char **paths) {}

void Engine::window_close_callback_(GLFWwindow *window) {}

void Engine::window_size_callback_(GLFWwindow *window, int width, int height) {}

void Engine::framebuffer_size_callback_(GLFWwindow *window, int width, int height) {}

void Engine::window_content_scale_callback_(GLFWwindow *window, float xscale, float yscale) {}

void Engine::window_pos_callback_(GLFWwindow *window, int xpos, int ypos) {}

void Engine::window_iconify_callback_(GLFWwindow *window, int iconified) {}

void Engine::window_maximize_callback_(GLFWwindow *window, int maximized) {}

void Engine::window_focus_callback_(GLFWwindow *window, int focused) {}

void Engine::window_refresh_callback_(GLFWwindow *window) {}

void Engine::monitor_callback_(GLFWmonitor *monitor, int event) {}

} // namespace goat
