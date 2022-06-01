#pragma once

#include <map>
#include <string>

namespace goat {

class InputMgr {
  friend class Application;
  friend class Engine;

public:
  struct {
    double x{0.0}, y{0.0};
    double px{0.0}, py{0.0};

    double dx{0.0}, dy{0.0};

    double sx{0.0}, sy{0.0};

    bool moved{false};
    bool entered{false};

    bool got_first_event{false};
  } mouse{};

  InputMgr() = default;

  bool pressed(const std::string &action);
  bool released(const std::string &action);
  bool down(const std::string &action, double interval=0.0, double delay=0.0);

private:
  struct ActionInfo {
    double time{0.0};
    bool pressed{false};
    bool down{false};
    bool released{false};

    bool repeat

    ActionInfo() = default;
  };

  std::map<std::string, ActionInfo> state_{};

  void update_(double dt);

  void key_callback_(int key, int scancode, int action, int mods);
  void cursor_position_callback_(double xpos, double ypos);
  void cursor_enter_callback_(int entered);
  void mouse_button_callback_(int button, int action, int mods);
  void scroll_callback_(double xoffset, double yoffset);
  void joystick_callback_(int jid, int event);

  std::string glfw_key_to_string_(int key);
  std::string glfw_button_to_string_(int button);
};

} // namespace goat
