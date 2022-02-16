#pragma once

#include "baphomet/util/time/time.hpp"

#include "fmt/ostream.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace baphomet {

enum class MsgEndpoint {
  Runner,
  Application,
  Window,
  Audio,
  Input,
  Timer,
  Tween,

  Undefined
};

std::ostream &operator<<(std::ostream &out, MsgEndpoint value);

enum class MsgCategory {
  /* RUNNER INITIALIZATION */
  RegisterGlfwCallbacks,

  /* GLFW CALLBACKS */
  KeyEvent,
  CursorPositionEvent,
  CursorEnterEvent,
  MouseButtonEvent,
  ScrollEvent,
  WindowSize,
  WindowFocus,

  /* GENERAL */
  Update
};

std::ostream &operator<<(std::ostream &out, MsgCategory value);

template<MsgCategory>
struct PayloadMap;

#define EVENT_TYPE_PAYLOAD(e, ...)                \
  template<> struct PayloadMap<MsgCategory::e> {  \
    using type = struct e ## Pay {                \
      __VA_ARGS__                                 \
      MsgEndpoint origin;                         \
    };                                            \
  };

EVENT_TYPE_PAYLOAD(RegisterGlfwCallbacks,
    GLFWwindow *window;
)

EVENT_TYPE_PAYLOAD(KeyEvent,
    int key;
    int scancode;
    int action;
    int mods;
)

EVENT_TYPE_PAYLOAD(CursorPositionEvent,
    double xpos;
    double ypos;
)

EVENT_TYPE_PAYLOAD(CursorEnterEvent,
    int entered;
)

EVENT_TYPE_PAYLOAD(MouseButtonEvent,
    int button;
    int action;
    int mods;
)

EVENT_TYPE_PAYLOAD(ScrollEvent,
    double xoffset;
    double yoffset;
)

EVENT_TYPE_PAYLOAD(WindowSize,
    int width;
    int height;
)

EVENT_TYPE_PAYLOAD(WindowFocus,
    int focused;
)

EVENT_TYPE_PAYLOAD(Update,
    Duration dt;
)

// don't leak this, it's just for convenience here
#undef EVENT_TYPE_PAYLOAD

} // namespace baphomet