#pragma once

#define GLFW_INCLUDE_NONE
#include "goat/util/enum_bitops.hpp"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include <mutex>
#include <string>

namespace goat {

enum class WBackend {gl, vulkan};

enum class WFlags {
  none       = 1 << 0,
  resizable  = 1 << 1,
  hidden     = 1 << 2,
  centered   = 1 << 3,
  fullscreen = 1 << 4,
  borderless = 1 << 5
};

struct WCfg {
  std::string title{"Baphomet Window"};
  glm::ivec2 size{1, 1};
  glm::ivec2 position{0, 0};

  int monitor{0};

  WFlags flags{WFlags::none};

  WBackend gfx_backend{WBackend::gl};
  glm::ivec2 gfx_version{3, 3};

  spdlog::level::level_enum log_level{spdlog::level::info};
};

class Window {
public:
  Window(const WCfg &cfg);
  ~Window();

  void set_should_close(bool should_close);
  void set_size(int w, int h);
  void set_w(int w);
  void set_h(int h);
  void set_pos(int x, int y);
  void set_x(int x);
  void set_y(int y);
  void set_vsync(bool vsync);
  void set_resizable(bool resizable);
  void set_visible(bool visible);

  bool should_close() const;
  glm::ivec2 size() const;
  int w() const;
  int h() const;
  glm::ivec2 pos() const;
  int x() const;
  int y() const;
  bool vsync() const;
  bool resizable() const;
  bool visible() const;

  GLFWwindow *glfw_handle() const;
  void make_context_current() const;
  void swap_buffers() const;

private:
  struct {
    GLFWwindow *handle{nullptr};
    bool borderless{false};
    bool vsync{false};
  } wm_{};

  void open_for_gl_(const WCfg &cfg);

  GLFWmonitor *get_monitor_(int monitor_num);
  void open_fullscreen_(const WCfg &cfg);
  void open_windowed_(const WCfg &cfg);

  static std::once_flag initialized_glfw_;
};

} // namespace goat

ENABLE_ENUM_BITOPS(goat::WFlags);
