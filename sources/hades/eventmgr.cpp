#include "hades/eventmgr.hpp"

namespace hades {

EventMgr::EventMgr(GLFWwindow *parent)
    : parent_(parent) {}

void EventMgr::set_mouse_locked(bool locked) {
    glfwSetInputMode(parent_, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    mouse.locked = locked;
}

void EventMgr::set_mouse_hidden(bool hidden) {
    glfwSetInputMode(parent_, GLFW_CURSOR, hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
    mouse.hidden = hidden;
}

bool EventMgr::raw_mouse_motion_supported() {
    return glfwRawMouseMotionSupported() == 1;
}

void EventMgr::set_raw_mouse_motion(bool raw) {
    glfwSetInputMode(parent_, GLFW_RAW_MOUSE_MOTION, raw ? GLFW_TRUE : GLFW_FALSE);
    mouse.raw_motion = raw;
}

bool EventMgr::pressed(const std::string &action) {
    return state_[action] && !prev_state_[action];
}

bool EventMgr::released(const std::string &action) {
    return !state_[action] && prev_state_[action];
}

bool EventMgr::down(const std::string &action, double interval, double delay) {
    if (interval <= 0.0 && delay <= 0.0)
        return state_[action];

    else if (repeat_state_.find(action) != repeat_state_.end())
        return repeat_state_[action].pressed;

    else if (state_[action]) {
        if (delay >= 0.0) {
            repeat_state_[action] = RepeatState {
                .time = interval,
                .interval = interval,
                .delay = delay,
                .delay_stage = true};
            return false;
        } else {
            repeat_state_[action] = RepeatState {
                .time = interval,
                .interval = interval};
            return true;
        }
    }

    return false;
}

void EventMgr::update_(double dt) {
    for (auto &p : state_)
        prev_state_[p.first] = p.second;

    mouse.dx = 0;
    mouse.dy = 0;
    mouse.sx = 0;
    mouse.sy = 0;

    for (auto &p : repeat_state_) {
        auto r = &p.second;
        r->pressed = false;

        if (r->delay_stage) {
            r->delay -= dt;
            if (r->delay <= 0.0) {
                r->delay_stage = false;
                r->pressed = true;
            }
        } else {
            r->time -= dt;
            if (r->time <= 0.0) {
                r->time += r->interval;
                r->pressed = true;
            }
        }
    }
}

void EventMgr::glfw_key_event_(int key, int scancode, int action, int mods) {
    auto key_str = glfw_key_to_str_(key);
    if (action == GLFW_PRESS)
        state_[key_str] = true;
    else if (action == GLFW_RELEASE) {
        state_[key_str] = false;
        repeat_state_.erase(key_str);
    }
}

void EventMgr::glfw_cursor_position_event_(double xpos, double ypos) {
    mouse.dx += xpos - mouse.x;  // += because multiple events can happen in a single frame
    mouse.dy += ypos - mouse.y;  // see previous comment
    mouse.x = xpos;
    mouse.y = ypos;

    if (!mouse.got_first_event) {
        mouse.got_first_event = true;
        mouse.dx = 0;
        mouse.dy = 0;
    }
}

void EventMgr::glfw_cursor_enter_event_(int entered) {
}

void EventMgr::glfw_mouse_button_event_(int button, int action, int mods) {
    auto button_str = glfw_button_to_str_(button);
    if (action == GLFW_PRESS)
        state_[button_str] = true;
    else if (action == GLFW_RELEASE) {
        state_[button_str] = false;
        repeat_state_.erase(button_str);
    }
}

void EventMgr::glfw_scroll_event_(double xoffset, double yoffset) {
    mouse.sx = xoffset;
    mouse.sy = yoffset;
}

std::string EventMgr::glfw_key_to_str_(int key) {
    static std::unordered_map<int, std::string> mapping {
        {GLFW_KEY_UNKNOWN, "unknown"},
        {GLFW_KEY_SPACE, "space"},
        {GLFW_KEY_APOSTROPHE, "apostrophe"},
        {GLFW_KEY_COMMA, "comma"},
        {GLFW_KEY_MINUS, "minus"},
        {GLFW_KEY_PERIOD, "period"},
        {GLFW_KEY_SLASH, "slash"},
        {GLFW_KEY_0, "0"},
        {GLFW_KEY_1, "1"},
        {GLFW_KEY_2, "2"},
        {GLFW_KEY_3, "3"},
        {GLFW_KEY_4, "4"},
        {GLFW_KEY_5, "5"},
        {GLFW_KEY_6, "6"},
        {GLFW_KEY_7, "7"},
        {GLFW_KEY_8, "8"},
        {GLFW_KEY_9, "9"},
        {GLFW_KEY_SEMICOLON, "semicolon"},
        {GLFW_KEY_EQUAL, "equal"},
        {GLFW_KEY_A, "a"},
        {GLFW_KEY_B, "b"},
        {GLFW_KEY_C, "c"},
        {GLFW_KEY_D, "d"},
        {GLFW_KEY_E, "e"},
        {GLFW_KEY_F, "f"},
        {GLFW_KEY_G, "g"},
        {GLFW_KEY_H, "h"},
        {GLFW_KEY_I, "i"},
        {GLFW_KEY_J, "j"},
        {GLFW_KEY_K, "k"},
        {GLFW_KEY_L, "l"},
        {GLFW_KEY_M, "m"},
        {GLFW_KEY_N, "n"},
        {GLFW_KEY_O, "o"},
        {GLFW_KEY_P, "p"},
        {GLFW_KEY_Q, "q"},
        {GLFW_KEY_R, "r"},
        {GLFW_KEY_S, "s"},
        {GLFW_KEY_T, "t"},
        {GLFW_KEY_U, "u"},
        {GLFW_KEY_V, "v"},
        {GLFW_KEY_W, "w"},
        {GLFW_KEY_X, "x"},
        {GLFW_KEY_Y, "y"},
        {GLFW_KEY_Z, "z"},
        {GLFW_KEY_LEFT_BRACKET, "left_bracket"},
        {GLFW_KEY_BACKSLASH, "backslash"},
        {GLFW_KEY_RIGHT_BRACKET, "right_bracket"},
        {GLFW_KEY_GRAVE_ACCENT, "grave_accent"},
        {GLFW_KEY_WORLD_1, "world_1"},
        {GLFW_KEY_WORLD_2, "world_2"},
        {GLFW_KEY_ESCAPE, "escape"},
        {GLFW_KEY_ENTER, "enter"},
        {GLFW_KEY_TAB, "tab"},
        {GLFW_KEY_BACKSPACE, "backspace"},
        {GLFW_KEY_INSERT, "insert"},
        {GLFW_KEY_DELETE, "delete"},
        {GLFW_KEY_RIGHT, "right"},
        {GLFW_KEY_LEFT, "left"},
        {GLFW_KEY_DOWN, "down"},
        {GLFW_KEY_UP, "up"},
        {GLFW_KEY_PAGE_UP, "page_up"},
        {GLFW_KEY_PAGE_DOWN, "page_down"},
        {GLFW_KEY_HOME, "home"},
        {GLFW_KEY_END, "end"},
        {GLFW_KEY_CAPS_LOCK, "caps_lock"},
        {GLFW_KEY_SCROLL_LOCK, "scroll_lock"},
        {GLFW_KEY_NUM_LOCK, "num_lock"},
        {GLFW_KEY_PRINT_SCREEN, "print_screen"},
        {GLFW_KEY_PAUSE, "pause"},
        {GLFW_KEY_F1, "f1"},
        {GLFW_KEY_F2, "f2"},
        {GLFW_KEY_F3, "f3"},
        {GLFW_KEY_F4, "f4"},
        {GLFW_KEY_F5, "f5"},
        {GLFW_KEY_F6, "f6"},
        {GLFW_KEY_F7, "f7"},
        {GLFW_KEY_F8, "f8"},
        {GLFW_KEY_F9, "f9"},
        {GLFW_KEY_F10, "f10"},
        {GLFW_KEY_F11, "f11"},
        {GLFW_KEY_F12, "f12"},
        {GLFW_KEY_F13, "f13"},
        {GLFW_KEY_F14, "f14"},
        {GLFW_KEY_F15, "f15"},
        {GLFW_KEY_F16, "f16"},
        {GLFW_KEY_F17, "f17"},
        {GLFW_KEY_F18, "f18"},
        {GLFW_KEY_F19, "f19"},
        {GLFW_KEY_F20, "f20"},
        {GLFW_KEY_F21, "f21"},
        {GLFW_KEY_F22, "f22"},
        {GLFW_KEY_F23, "f23"},
        {GLFW_KEY_F24, "f24"},
        {GLFW_KEY_F25, "f25"},
        {GLFW_KEY_KP_0, "kp_0"},
        {GLFW_KEY_KP_1, "kp_1"},
        {GLFW_KEY_KP_2, "kp_2"},
        {GLFW_KEY_KP_3, "kp_3"},
        {GLFW_KEY_KP_4, "kp_4"},
        {GLFW_KEY_KP_5, "kp_5"},
        {GLFW_KEY_KP_6, "kp_6"},
        {GLFW_KEY_KP_7, "kp_7"},
        {GLFW_KEY_KP_8, "kp_8"},
        {GLFW_KEY_KP_9, "kp_9"},
        {GLFW_KEY_KP_DECIMAL, "kp_decimal"},
        {GLFW_KEY_KP_DIVIDE, "kp_divide"},
        {GLFW_KEY_KP_MULTIPLY, "kp_multiply"},
        {GLFW_KEY_KP_SUBTRACT, "kp_subtract"},
        {GLFW_KEY_KP_ADD, "kp_add"},
        {GLFW_KEY_KP_ENTER, "kp_enter"},
        {GLFW_KEY_KP_EQUAL, "kp_equal"},
        {GLFW_KEY_LEFT_SHIFT, "left_shift"},
        {GLFW_KEY_LEFT_CONTROL, "left_control"},
        {GLFW_KEY_LEFT_ALT, "left_alt"},
        {GLFW_KEY_LEFT_SUPER, "left_super"},
        {GLFW_KEY_RIGHT_SHIFT, "right_shift"},
        {GLFW_KEY_RIGHT_CONTROL, "right_control"},
        {GLFW_KEY_RIGHT_ALT, "right_alt"},
        {GLFW_KEY_RIGHT_SUPER, "right_super"},
        {GLFW_KEY_MENU, "menu"}};

    return mapping[key];
}

std::string EventMgr::glfw_button_to_str_(int button) {
    static std::unordered_map<int, std::string> mapping {
        {GLFW_MOUSE_BUTTON_1, "mb_left"},
        {GLFW_MOUSE_BUTTON_2, "mb_right"},
        {GLFW_MOUSE_BUTTON_3, "mb_middle"},
        {GLFW_MOUSE_BUTTON_4, "mb_4"},
        {GLFW_MOUSE_BUTTON_5, "mb_5"},
        {GLFW_MOUSE_BUTTON_6, "mb_6"},
        {GLFW_MOUSE_BUTTON_7, "mb_7"},
        {GLFW_MOUSE_BUTTON_8, "mb_8"}};

    return mapping[button];
}

} // namespace hades