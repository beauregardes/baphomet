#include "baphomet/app/internal/messenger_config.hpp"

namespace baphomet {

// TODO: Can we do this without having to duplicate the enum typed out
//   each time? There aren't many so it's not really a problem but it
//   absolutely feels like code smell.
std::ostream& operator<<(std::ostream& out, const MsgEndpoint value) {
  std::string s;
#define PROCESS_VAL(p) case(p): s = #p; break;
  switch (value) {
    using enum MsgEndpoint;

    PROCESS_VAL(Runner)
    PROCESS_VAL(Application)
    PROCESS_VAL(Window)
    PROCESS_VAL(Audio)
    PROCESS_VAL(Input)
    PROCESS_VAL(Timer)
    PROCESS_VAL(Tween)

    PROCESS_VAL(Undefined)
  }
#undef PROCESS_VAL

  return out << s;
}

// TODO: Can we do this without having to duplicate the enum typed out
//   each time? There aren't many so it's not really a problem but it
//   absolutely feels like code smell.
std::ostream& operator<<(std::ostream& out, const MsgCategory value) {
  std::string s;
#define PROCESS_VAL(p) case(p): s = #p; break;
  switch(value){
    using enum MsgCategory;

    PROCESS_VAL(RegisterGlfwCallbacks)

    PROCESS_VAL(KeyEvent)
    PROCESS_VAL(CursorPositionEvent)
    PROCESS_VAL(CursorEnterEvent)
    PROCESS_VAL(MouseButtonEvent)
    PROCESS_VAL(ScrollEvent)
    PROCESS_VAL(WindowSize)
    PROCESS_VAL(WindowFocus)

    PROCESS_VAL(Update)
  }
#undef PROCESS_VAL

  return out << s;
}

} // namespace baphomet