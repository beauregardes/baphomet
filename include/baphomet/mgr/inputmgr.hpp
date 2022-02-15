#pragma once

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/util/time/time.hpp"

#include "fmt/format.h"
#include "fmt/chrono.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace baphomet {

class InputMgr {
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

  InputMgr(GLFWwindow *parent, std::shared_ptr<Messenger> msgr);

  void set_mouse_locked(bool locked);
  void set_mouse_hidden(bool hidden);

  bool raw_mouse_motion_supported();
  void set_raw_mouse_motion(bool raw);

  void bind(const std::string &name, const std::string &action);
  void unbind(const std::string &name, const std::string &action);

  bool pressed(const std::string &name);
  bool released(const std::string &name);
  bool down(const std::string &name, Duration interval = Duration(0), Duration delay = Duration(0));

  template <typename ...Args>
  bool sequence(const std::string &name, const Args &...args);

private:
  std::shared_ptr<Messenger> msgr_;

  GLFWwindow *parent_ {nullptr};

  struct RepeatState {
    Duration time{0};
    Duration interval{0};
    Duration delay{0};
    bool delay_stage {false};
    bool pressed {false};
  };

  struct SequenceStep {
    Duration remaining{0};
    std::string name{};
  };
  struct Sequence {
    std::size_t idx{0};
    std::vector<SequenceStep> steps{};
  };

  std::unordered_map<std::string, std::unordered_set<std::string>> binds_{};
  std::unordered_map<std::string, bool> state_{};
  std::unordered_map<std::string, bool> prev_state_{};
  std::unordered_map<std::string, RepeatState> repeat_state_{};
  std::unordered_map<std::string, Sequence> sequences_{};

  template <typename ...Args>
  std::string get_sequence_tag_(Duration d, const std::string &name, const Args &... args);

  template <typename ...Args>
  void get_sequence_(Sequence &out, Duration d, const std::string &name, const Args &... args);

  bool check_sequence_(const std::string &tag);

  void update_(Duration dt);

  void received_message_(const MsgCat &category, const std::any &payload);

  void glfw_key_event_(int key, int scancode, int action, int mods);
  void glfw_cursor_position_event_(double xpos, double ypos);
  void glfw_cursor_enter_event_(int entered);
  void glfw_mouse_button_event_(int button, int action, int mods);
  void glfw_scroll_event_(double xoffset, double yoffset);

  static std::string glfw_key_to_str_(int key);
  static std::string glfw_button_to_str_(int button);

  static const std::unordered_set<std::string> &all_actions_();
};

template <typename ...Args>
bool InputMgr::sequence(const std::string &name, const Args &...args) {
  if constexpr (sizeof...(args) >= 2) {
    auto tag = name + get_sequence_tag_(args...);
    if (sequences_.contains(tag))
      return check_sequence_(tag);
    else if (pressed(name)) {
      sequences_[tag] = Sequence();
      get_sequence_(sequences_[tag], args...);
    }

  } else if (pressed(name))
    return true;

  return false;
}

template <typename ...Args>
std::string InputMgr::get_sequence_tag_(Duration d, const std::string &name, const Args &... args) {
  auto comp = fmt::format("_{}{}", d, name);

  if constexpr (sizeof...(args) >= 2)
    return comp + get_sequence_tag_(args...);
  return comp;
}

template <typename ...Args>
void InputMgr::get_sequence_(Sequence &out, Duration d, const std::string &name, const Args &... args) {
  out.steps.emplace_back(SequenceStep{d, name});

  if constexpr (sizeof...(args) >= 2)
    get_sequence_(out, args...);
}

} // namespace baphomet