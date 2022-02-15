#pragma once

#include "baphomet/app/application.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <concepts>

namespace baphomet {

template<typename T>
concept DerivesHadesApplication = std::is_base_of_v<Application, T>;

class Runner {
public:
  Runner(spdlog::level::level_enum = spdlog::level::info);
  ~Runner();

  template<DerivesHadesApplication T>
  Runner &open(const WCfg &cfg);

  Runner &init_gl(glm::ivec2 glversion = {3, 3});

  void start();

private:
  std::shared_ptr<Messenger> msgr_{nullptr};

  struct {
    glm::ivec2 glversion{};
    WCfg cfg{};
  } open_params_{};

  std::unique_ptr<Application> application_{nullptr};

  GLFWwindow *glfw_window_{nullptr};

  void received_message_(const MsgCat &category, const std::any &payload);

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
  application_->msgr_ = msgr_;
  open_params_.cfg = cfg;
  return *this;
}

} // namespace baphomet
