#pragma once

#include "goat/application.hpp"
#include "goat/configs.hpp"
#include <concepts>

namespace goat {

template<typename T>
concept DerivesGoatApplication = std::is_base_of_v<Application, T>;

class Engine {
public:
  explicit Engine(const ECfg &cfg);

  template <DerivesGoatApplication T>
  void open(const WCfg &cfg);

private:
  const ECfg create_cfg{};

  std::unique_ptr<Application> application_{nullptr};

  void init_backend_();
  void init_gl_();

  void set_glfw_user_pointer_();
  void register_glfw_callbacks_();

  void run_();

  // GLFW event callbacks
  static void key_callback_(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursor_position_callback_(GLFWwindow* window, double xpos, double ypos);
  static void cursor_enter_callback_(GLFWwindow* window, int entered);
  static void mouse_button_callback_(GLFWwindow* window, int button, int action, int mods);
  static void scroll_callback_(GLFWwindow* window, double xoffset, double yoffset);
  static void joystick_callback_(int jid, int event);
  static void drop_callback_(GLFWwindow* window, int count, const char** paths);

  // GLFW window callbacks
  static void window_close_callback_(GLFWwindow* window);
  static void window_size_callback_(GLFWwindow* window, int width, int height);
  static void framebuffer_size_callback_(GLFWwindow* window, int width, int height);
  static void window_content_scale_callback_(GLFWwindow* window, float xscale, float yscale);
  static void window_pos_callback_(GLFWwindow* window, int xpos, int ypos);
  static void window_iconify_callback_(GLFWwindow* window, int iconified);
  static void window_maximize_callback_(GLFWwindow* window, int maximized);
  static void window_focus_callback_(GLFWwindow* window, int focused);
  static void window_refresh_callback_(GLFWwindow* window);

  // GLFW monitor callbacks
  static void monitor_callback_(GLFWmonitor* monitor, int event);
};

template <DerivesGoatApplication T>
void Engine::open(const WCfg &cfg) {
  application_ = std::unique_ptr<Application>(new T());

  application_->open_(create_cfg, cfg);
  set_glfw_user_pointer_();
  register_glfw_callbacks_();

  init_backend_();
  run_();
}

} // namespace goat
