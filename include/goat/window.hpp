#pragma once

#define GLFW_INCLUDE_NONE
#include "goat/util/enum_bitops.hpp"
#include "goat/configs.hpp"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include <mutex>
#include <string>

namespace goat {

class Window {
public:
  Window(const ECfg &engine_create_cfg, const WCfg &cfg);
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

  void open_(const ECfg &engine_create_cfg, const WCfg &cfg);
  void open_for_gl_(const ECfg &engine_create_cfg, const WCfg &cfg);

  GLFWmonitor *get_monitor_(int monitor_num);
  void open_fullscreen_(const WCfg &cfg);
  void open_windowed_(const WCfg &cfg);

  static std::once_flag initialized_glfw_;
};

} // namespace goat

ENABLE_ENUM_BITOPS(goat::WFlags);
