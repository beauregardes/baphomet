#pragma once

#include <map>
#include <string>

namespace goat {

class InputMgr {
public:
  struct {
    int x{0}, y{0};
  } mouse;

private:
  std::map<std::string, bool> curr_{};
  std::map<std::string, bool> prev_{};
};

} // namespace goat
