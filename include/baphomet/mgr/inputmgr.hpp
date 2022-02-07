#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <string>
#include <unordered_map>

namespace baphomet {

class InputMgr {
  friend class Runner;

public:
  struct {
    double x{0.0}, y{0.0};
    double px{0.0}, py{0.0};
    double dx{0.0}, dy{0.0};
    double sx{0.0}, sy{0.0};

    bool moved{false};
    bool entered{false};

    bool got_first_event{false};

    bool locked{false};
    bool hidden{false};
    bool raw_motion{false};
  } mouse;

  InputMgr(GLFWwindow *parent);

  void set_mouse_locked(bool locked);
  void set_mouse_hidden(bool hidden);

  bool raw_mouse_motion_supported();
  void set_raw_mouse_motion(bool raw);

  bool pressed(const std::string &action);
  bool released(const std::string &action);
  bool down(const std::string &action, double interval = 0.0, double delay = 0.0);

private:
  GLFWwindow *parent_ {nullptr};

  struct RepeatState {
    double time;
    double interval;
    double delay {0.0};
    bool delay_stage {false};
    bool pressed {false};
  };

  std::unordered_map<std::string, bool> state_ {};
  std::unordered_map<std::string, bool> prev_state_ {};
  std::unordered_map<std::string, RepeatState> repeat_state_ {};

  void update_(double dt);

  void glfw_key_event_(int key, int scancode, int action, int mods);
  void glfw_cursor_position_event_(double xpos, double ypos);
  void glfw_cursor_enter_event_(int entered);
  void glfw_mouse_button_event_(int button, int action, int mods);
  void glfw_scroll_event_(double xoffset, double yoffset);

  std::string glfw_key_to_str_(int key);
  std::string glfw_button_to_str_(int button);
};

} // namespace baphomet