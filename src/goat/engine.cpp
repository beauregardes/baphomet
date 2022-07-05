#include "goat/util/ticker.hpp"
#include "goat/engine.hpp"
#include "glbinding/glbinding.h"
#include "spdlog/spdlog.h"

namespace goat {

Engine::Engine(const ECfg &cfg) : create_cfg(cfg) {
  spdlog::set_level(cfg.log_level);
}

void Engine::init_backend_() {
#if defined(GOAT_BACKEND_GL)
  init_gl_();
#else
  spdlog::critical("Backend NYI");
  std::exit(EXIT_FAILURE);
#endif
}

void Engine::init_gl_() {
  application_->window->make_context_current();
  glbinding::initialize(glfwGetProcAddress);
}

void Engine::set_glfw_user_pointer_() {
  auto window = application_->window->glfw_handle();
  glfwSetWindowUserPointer(window, this);
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
  application_->initialize_();

  auto ticker = Ticker();
  do {
    application_->update_(ticker.dt_sec());
    application_->draw_();

    application_->update_event_mgrs_(ticker.dt_sec());
    glfwPollEvents();

    ticker.tick();
  } while (!application_->window->should_close());
}

/******************
 * GLFW CALLBACKS *
 ******************/

void Engine::key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
  auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  engine->application_->input->key_callback_(key, scancode, action, mods);
}

void Engine::cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {
  auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  engine->application_->input->cursor_position_callback_(xpos, ypos);
}

void Engine::cursor_enter_callback_(GLFWwindow *window, int entered) {
  auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  engine->application_->input->cursor_enter_callback_(entered);
}

void Engine::mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {
  auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  engine->application_->input->mouse_button_callback_(button, action, mods);
}

void Engine::scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {
  auto engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
  engine->application_->input->scroll_callback_(xoffset, yoffset);
}

void Engine::joystick_callback_(int jid, int event) { /* TODO: Attach this to the InputMgr somehow */ }

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
