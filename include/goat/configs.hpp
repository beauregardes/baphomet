#pragma once

#include "glm/vec2.hpp"
#include "spdlog/spdlog.h"
#include <string>

#ifndef GOAT_BACKEND_GL
#define GOAT_BACKEND_GL
#endif

namespace goat {

//enum class Backend {gl, vulkan};

struct ECfg {
//  Backend backend{Backend::gl};
  glm::ivec2 backend_version{3, 3};
  spdlog::level::level_enum log_level{spdlog::level::info};
};

enum class WFlags {
  none =       1 << 0,
  resizable =  1 << 1,
  hidden =     1 << 2,
  centered =   1 << 3,
  fullscreen = 1 << 4,
  borderless = 1 << 5
};

struct WCfg {
  std::string title{"Baphomet Window"};
  glm::ivec2 size{1, 1};
  glm::ivec2 position{0, 0};

  int monitor{0};

  WFlags flags{WFlags::none};
};

} // namespace goat
