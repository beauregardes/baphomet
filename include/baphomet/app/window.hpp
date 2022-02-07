#pragma once

#include "baphomet/gfx/gl/framebuffer.hpp"
#include "baphomet/util/enum_bitmask_ops.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <memory>
#include <string>

namespace baphomet {

enum class WFlags {
  none       = 1 << 0,
  resizable  = 1 << 1,
  vsync      = 1 << 2,
  hidden     = 1 << 3,
  centered   = 1 << 4,
  fullscreen = 1 << 5,
  borderless = 1 << 6
};

struct WCfg {
  std::string title;

  glm::ivec2 size{1, 1};
  glm::ivec2 position{0, 0};

  int monitor{0};

  WFlags flags{WFlags::none};
};

class Window {
  friend class Application;
  friend class Runner;

public:
  void set_size(int width, int height);
  glm::ivec2 size() const;
  int w() const;
  int h() const;

  void set_pos(int x, int y);
  glm::ivec2 pos() const;
  int x() const;
  int y() const;

  void center(int monitor_num = 0);

  void set_vsync(bool vsync);
  bool vsync() const;

  void set_auto_iconify(bool auto_iconify);
  bool auto_iconify();

  void set_floating(bool floating);
  bool floating();

  void set_resizable(bool resizable);
  bool resizable();

  void set_visible(bool visible);
  bool visible();

  void set_decorated(bool decorated);
  bool decorated();

  glm::mat4 projection() const;

private:
  GLFWwindow *glfw_window_{nullptr};

  std::unique_ptr<gl::Framebuffer> fbo_{nullptr};
  void create_fbo_();

  struct {
    bool borderless{false};
    bool vsync{false};
  } wm_info_;

  void open_(const WCfg &cfg, glm::ivec2 glversion);
  void close_();

  GLFWmonitor *get_monitor_(int monitor_num);
  void open_fullscreen_(const WCfg &cfg);
  void open_windowed_(const WCfg &cfg);
};

} // namespace baphomet

ENABLE_BITMASK_OPERATORS(baphomet::WFlags);