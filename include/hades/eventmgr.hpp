#ifndef HADES_EVENTMGR_HPP
#define HADES_EVENTMGR_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "hades/internal/ts_queue.hpp"

#include <string>
#include <unordered_map>

namespace hades {

class EventMgr {
    friend class Application;
    friend class Runner;

public:
    struct {
        double x {0}, y {0};
        double dx {0}, dy {0};
        double sx {0}, sy {0};

        bool got_first_event {false};

        bool locked {false};
        bool hidden {false};
        bool raw_motion {false};
    } mouse;

    EventMgr(GLFWwindow *parent);

    void set_mouse_locked(bool locked);
    void set_mouse_hidden(bool hidden);

    bool raw_mouse_motion_supported();
    void set_raw_mouse_motion(bool raw);

    bool pressed(const std::string &action);
    bool released(const std::string &action);
    bool down(const std::string &action, double interval = 0.0, double delay = 0.0);

private:
    GLFWwindow *parent_ {nullptr};

    TSQueue<std::pair<std::string, bool>> state_updates_{};

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

} // namespace hades

#endif //HADES_EVENTMGR_HPP
