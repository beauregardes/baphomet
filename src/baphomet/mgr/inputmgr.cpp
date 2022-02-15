#include "baphomet/mgr/inputmgr.hpp"

#include "spdlog/spdlog.h"

namespace baphomet {

InputMgr::InputMgr(GLFWwindow *parent, std::shared_ptr<Messenger> msgr)
    : msgr_(msgr), parent_(parent) {

  msgr_->register_endpoint("INPUT-MGR", [&](const MsgCat &category, const std::any &payload) {
    received_message_(category, payload);
  });

  for (const auto &action : all_actions_())
    bind(action, action);
}

void InputMgr::set_mouse_locked(bool locked) {
  glfwSetInputMode(parent_, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
  mouse.locked = locked;
}

void InputMgr::set_mouse_hidden(bool hidden) {
  glfwSetInputMode(parent_, GLFW_CURSOR, hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
  mouse.hidden = hidden;
}

bool InputMgr::raw_mouse_motion_supported() {
  return glfwRawMouseMotionSupported() == GLFW_TRUE;
}

void InputMgr::set_raw_mouse_motion(bool raw) {
  glfwSetInputMode(parent_, GLFW_RAW_MOUSE_MOTION, raw ? GLFW_TRUE : GLFW_FALSE);
  mouse.raw_motion = raw;
}

void InputMgr::bind(const std::string &name, const std::string &action) {
  binds_[name].insert(action);
}

void InputMgr::unbind(const std::string &name, const std::string &action) {
  binds_[name].erase(action);
}

bool InputMgr::pressed(const std::string &name) {
  if (!binds_[name].empty())
    for (const auto &b : binds_[name])
      if (state_[b] && !prev_state_[b])
        return true;
  return false;
}

bool InputMgr::released(const std::string &name) {
  if (!binds_[name].empty())
    for (const auto &b : binds_[name])
      if (!state_[b] && prev_state_[b])
        return true;
  return false;
}

bool InputMgr::down(const std::string &name, Duration interval, Duration delay) {
  if (interval <= Duration(0) && delay <= Duration(0)) {
    for (const auto &b : binds_[name])
      if (state_[b])
        return true;

  } else {
    if (binds_[name].empty())
      return false;

    for (const auto &b : binds_[name]) {
      if (repeat_state_.find(b) != repeat_state_.end())
        return repeat_state_[b].pressed;

      else if (state_[b]) {
        if (delay >= Duration(0)) {
          repeat_state_[b] = RepeatState{
              .time = interval,
              .interval = interval,
              .delay = delay,
              .delay_stage = true
          };
          return true;
        } else {
          repeat_state_[b] = RepeatState{
              .time = interval,
              .interval = interval
          };
          return true;
        }
      }
    }
  }

  return false;
}

bool InputMgr::check_sequence_(const std::string &tag) {
  auto &s = sequences_[tag];

  // Timed out but not removed yet
  if (s.steps[s.idx].remaining <= sec(0.0))
    return false;

  // Next check of we should move to the next item (or finish)
  else if (pressed(s.steps[s.idx].name)) {
    if (s.idx == s.steps.size() - 1) {
      s.steps[s.idx].remaining = sec(0.0);  // Mark for deletion
      return true;
    }
    s.idx++;
  }

  return false;
}

void InputMgr::update_(Duration dt) {
  for (auto &p: state_)
    prev_state_[p.first] = p.second;

  mouse.px = mouse.x;
  mouse.py = mouse.y;
  mouse.dx = 0;
  mouse.dy = 0;
  mouse.sx = 0;
  mouse.sy = 0;
  mouse.moved = false;

  for (auto &p: repeat_state_) {
    auto r = &p.second;
    r->pressed = false;

    if (r->delay_stage) {
      r->delay -= dt;
      if (r->delay <= Duration(0)) {
        r->delay_stage = false;
        r->pressed = true;
      }
    } else {
      r->time -= dt;
      if (r->time <= Duration(0)) {
        r->time += r->interval;
        r->pressed = true;
      }
    }
  }

  for (auto it = sequences_.begin(); it != sequences_.end(); ) {
    it->second.steps[it->second.idx].remaining -= dt;

    if (it->second.steps[it->second.idx].remaining <= sec(0.0)) {
      it = sequences_.erase(it);
      continue;
    }

    it++;
  }
}

void InputMgr::received_message_(const MsgCat &category, const std::any &payload) {
  switch (category) {
    using enum MsgCat;

    case KeyEvent: {
      auto p = Messenger::extract_payload<KeyEvent>(payload);
      glfw_key_event_(p.key, p.scancode, p.action, p.mods);
    }
      break;

    case CursorPositionEvent: {
      auto p = Messenger::extract_payload<CursorPositionEvent>(payload);
      glfw_cursor_position_event_(p.xpos, p.ypos);
    }
      break;

    case CursorEnterEvent: {
      auto p = Messenger::extract_payload<CursorEnterEvent>(payload);
      glfw_cursor_enter_event_(p.entered);
    }
      break;

    case MouseButtonEvent: {
      auto p = Messenger::extract_payload<MouseButtonEvent>(payload);
      glfw_mouse_button_event_(p.button, p.action, p.mods);
    }
      break;

    case ScrollEvent: {
      auto p = Messenger::extract_payload<ScrollEvent>(payload);
      glfw_scroll_event_(p.xoffset, p.yoffset);
    }
      break;

    case Update: {
      auto p = Messenger::extract_payload<Update>(payload);
      update_(p.dt);
    }
      break;

    default:
      spdlog::error("INPUT-MGR: Unhandled message category: '{}'", category);
  }
}

void InputMgr::glfw_key_event_(int key, int scancode, int action, int mods) {
  auto key_str = glfw_key_to_str_(key);
  if (action == GLFW_PRESS)
    state_[key_str] = true;
  else if (action == GLFW_RELEASE) {
    state_[key_str] = false;
    repeat_state_.erase(key_str);
  }
}

void InputMgr::glfw_cursor_position_event_(double xpos, double ypos) {
  mouse.dx += xpos - mouse.x;  // += because multiple events can happen in a single frame
  mouse.dy += ypos - mouse.y;  // see previous comment
  mouse.x = xpos;
  mouse.y = ypos;
  mouse.moved = true;

  if (!mouse.got_first_event) {
    mouse.got_first_event = true;
    mouse.dx = 0;
    mouse.dy = 0;
  }
}

void InputMgr::glfw_cursor_enter_event_(int entered) {
  mouse.entered = entered == 1;
}

void InputMgr::glfw_mouse_button_event_(int button, int action, int mods) {
  auto button_str = glfw_button_to_str_(button);
  if (action == GLFW_PRESS)
    state_[button_str] = true;
  else if (action == GLFW_RELEASE) {
    state_[button_str] = false;
    repeat_state_.erase(button_str);
  }
}

void InputMgr::glfw_scroll_event_(double xoffset, double yoffset) {
  mouse.sx = xoffset;
  mouse.sy = yoffset;
}

std::string InputMgr::glfw_key_to_str_(int key) {
  static std::unordered_map<int, std::string> mapping{
      {GLFW_KEY_UNKNOWN,       "unknown"},
      {GLFW_KEY_SPACE,         "space"},
      {GLFW_KEY_APOSTROPHE,    "apostrophe"},
      {GLFW_KEY_COMMA,         "comma"},
      {GLFW_KEY_MINUS,         "minus"},
      {GLFW_KEY_PERIOD,        "period"},
      {GLFW_KEY_SLASH,         "slash"},
      {GLFW_KEY_0,             "0"},
      {GLFW_KEY_1,             "1"},
      {GLFW_KEY_2,             "2"},
      {GLFW_KEY_3,             "3"},
      {GLFW_KEY_4,             "4"},
      {GLFW_KEY_5,             "5"},
      {GLFW_KEY_6,             "6"},
      {GLFW_KEY_7,             "7"},
      {GLFW_KEY_8,             "8"},
      {GLFW_KEY_9,             "9"},
      {GLFW_KEY_SEMICOLON,     "semicolon"},
      {GLFW_KEY_EQUAL,         "equal"},
      {GLFW_KEY_A,             "a"},
      {GLFW_KEY_B,             "b"},
      {GLFW_KEY_C,             "c"},
      {GLFW_KEY_D,             "d"},
      {GLFW_KEY_E,             "e"},
      {GLFW_KEY_F,             "f"},
      {GLFW_KEY_G,             "g"},
      {GLFW_KEY_H,             "h"},
      {GLFW_KEY_I,             "i"},
      {GLFW_KEY_J,             "j"},
      {GLFW_KEY_K,             "k"},
      {GLFW_KEY_L,             "l"},
      {GLFW_KEY_M,             "m"},
      {GLFW_KEY_N,             "n"},
      {GLFW_KEY_O,             "o"},
      {GLFW_KEY_P,             "p"},
      {GLFW_KEY_Q,             "q"},
      {GLFW_KEY_R,             "r"},
      {GLFW_KEY_S,             "s"},
      {GLFW_KEY_T,             "t"},
      {GLFW_KEY_U,             "u"},
      {GLFW_KEY_V,             "v"},
      {GLFW_KEY_W,             "w"},
      {GLFW_KEY_X,             "x"},
      {GLFW_KEY_Y,             "y"},
      {GLFW_KEY_Z,             "z"},
      {GLFW_KEY_LEFT_BRACKET,  "left_bracket"},
      {GLFW_KEY_BACKSLASH,     "backslash"},
      {GLFW_KEY_RIGHT_BRACKET, "right_bracket"},
      {GLFW_KEY_GRAVE_ACCENT,  "grave_accent"},
      {GLFW_KEY_WORLD_1,       "world_1"},
      {GLFW_KEY_WORLD_2,       "world_2"},
      {GLFW_KEY_ESCAPE,        "escape"},
      {GLFW_KEY_ENTER,         "enter"},
      {GLFW_KEY_TAB,           "tab"},
      {GLFW_KEY_BACKSPACE,     "backspace"},
      {GLFW_KEY_INSERT,        "insert"},
      {GLFW_KEY_DELETE,        "delete"},
      {GLFW_KEY_RIGHT,         "right"},
      {GLFW_KEY_LEFT,          "left"},
      {GLFW_KEY_DOWN,          "down"},
      {GLFW_KEY_UP,            "up"},
      {GLFW_KEY_PAGE_UP,       "page_up"},
      {GLFW_KEY_PAGE_DOWN,     "page_down"},
      {GLFW_KEY_HOME,          "home"},
      {GLFW_KEY_END,           "end"},
      {GLFW_KEY_CAPS_LOCK,     "caps_lock"},
      {GLFW_KEY_SCROLL_LOCK,   "scroll_lock"},
      {GLFW_KEY_NUM_LOCK,      "num_lock"},
      {GLFW_KEY_PRINT_SCREEN,  "print_screen"},
      {GLFW_KEY_PAUSE,         "pause"},
      {GLFW_KEY_F1,            "f1"},
      {GLFW_KEY_F2,            "f2"},
      {GLFW_KEY_F3,            "f3"},
      {GLFW_KEY_F4,            "f4"},
      {GLFW_KEY_F5,            "f5"},
      {GLFW_KEY_F6,            "f6"},
      {GLFW_KEY_F7,            "f7"},
      {GLFW_KEY_F8,            "f8"},
      {GLFW_KEY_F9,            "f9"},
      {GLFW_KEY_F10,           "f10"},
      {GLFW_KEY_F11,           "f11"},
      {GLFW_KEY_F12,           "f12"},
      {GLFW_KEY_F13,           "f13"},
      {GLFW_KEY_F14,           "f14"},
      {GLFW_KEY_F15,           "f15"},
      {GLFW_KEY_F16,           "f16"},
      {GLFW_KEY_F17,           "f17"},
      {GLFW_KEY_F18,           "f18"},
      {GLFW_KEY_F19,           "f19"},
      {GLFW_KEY_F20,           "f20"},
      {GLFW_KEY_F21,           "f21"},
      {GLFW_KEY_F22,           "f22"},
      {GLFW_KEY_F23,           "f23"},
      {GLFW_KEY_F24,           "f24"},
      {GLFW_KEY_F25,           "f25"},
      {GLFW_KEY_KP_0,          "kp_0"},
      {GLFW_KEY_KP_1,          "kp_1"},
      {GLFW_KEY_KP_2,          "kp_2"},
      {GLFW_KEY_KP_3,          "kp_3"},
      {GLFW_KEY_KP_4,          "kp_4"},
      {GLFW_KEY_KP_5,          "kp_5"},
      {GLFW_KEY_KP_6,          "kp_6"},
      {GLFW_KEY_KP_7,          "kp_7"},
      {GLFW_KEY_KP_8,          "kp_8"},
      {GLFW_KEY_KP_9,          "kp_9"},
      {GLFW_KEY_KP_DECIMAL,    "kp_decimal"},
      {GLFW_KEY_KP_DIVIDE,     "kp_divide"},
      {GLFW_KEY_KP_MULTIPLY,   "kp_multiply"},
      {GLFW_KEY_KP_SUBTRACT,   "kp_subtract"},
      {GLFW_KEY_KP_ADD,        "kp_add"},
      {GLFW_KEY_KP_ENTER,      "kp_enter"},
      {GLFW_KEY_KP_EQUAL,      "kp_equal"},
      {GLFW_KEY_LEFT_SHIFT,    "left_shift"},
      {GLFW_KEY_LEFT_CONTROL,  "left_control"},
      {GLFW_KEY_LEFT_ALT,      "left_alt"},
      {GLFW_KEY_LEFT_SUPER,    "left_super"},
      {GLFW_KEY_RIGHT_SHIFT,   "right_shift"},
      {GLFW_KEY_RIGHT_CONTROL, "right_control"},
      {GLFW_KEY_RIGHT_ALT,     "right_alt"},
      {GLFW_KEY_RIGHT_SUPER,   "right_super"},
      {GLFW_KEY_MENU,          "menu"}
  };

  return mapping[key];
}

std::string InputMgr::glfw_button_to_str_(int button) {
  static std::unordered_map<int, std::string> mapping{
      {GLFW_MOUSE_BUTTON_1, "mb_left"},
      {GLFW_MOUSE_BUTTON_2, "mb_right"},
      {GLFW_MOUSE_BUTTON_3, "mb_middle"},
      {GLFW_MOUSE_BUTTON_4, "mb_4"},
      {GLFW_MOUSE_BUTTON_5, "mb_5"},
      {GLFW_MOUSE_BUTTON_6, "mb_6"},
      {GLFW_MOUSE_BUTTON_7, "mb_7"},
      {GLFW_MOUSE_BUTTON_8, "mb_8"}
  };

  return mapping[button];
}

const std::unordered_set<std::string> &InputMgr::all_actions_() {
  static std::unordered_set<std::string> actions = {
      "unknown", "space", "apostrophe", "comma", "minus", "period", "slash", "0", "1", "2", "3", "4", "5", "6", "7",
      "8", "9", "semicolon", "equal", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p",
      "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "left_bracket", "backslash", "right_bracket", "grave_accent",
      "world_1", "world_2", "escape", "enter", "tab", "backspace", "insert", "delete", "right", "left", "down", "up",
      "page_up", "page_down", "home", "end", "caps_lock", "scroll_lock", "num_lock", "print_screen", "pause", "f1",
      "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18",
      "f19", "f20", "f21", "f22", "f23", "f24", "f25", "kp_0", "kp_1", "kp_2", "kp_3", "kp_4", "kp_5", "kp_6", "kp_7",
      "kp_8", "kp_9", "kp_decimal", "kp_divide", "kp_multiply", "kp_subtract", "kp_add", "kp_enter", "kp_equal",
      "left_shift", "left_control", "left_alt", "left_super", "right_shift", "right_control", "right_alt",
      "right_super", "menu", "mb_left", "mb_right", "mb_middle", "mb_4", "mb_5", "mb_6", "mb_7", "mb_8"
  };

  return actions;
}

} // namespace baphomet