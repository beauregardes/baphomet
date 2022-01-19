#ifndef HADES_WINDOW_HPP
#define HADES_WINDOW_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "gl/framebuffer.hpp"

#include "hades/gfxmgr.hpp"
#include "hades/internal/bitmask_enum.hpp"
#include "hades/util/framecounter.hpp"
#include "hades/util/platform.hpp"
#include "hades/util/ticker.hpp"
#include "hades/util/timermgr.hpp"
#include "hades/inputmgr.hpp"

#if defined(HADES_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <memory>
#include <string>

namespace hades {

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

  bool win32_force_light_mode{false};
  bool win32_force_dark_mode{false};
};

class Window {
  friend class Application;
  friend class Runner;

public:
  ~Window();

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

  void set_vsync(bool vsync);
  bool vsync();

  void set_opacity(float opacity);
  float opacity();

  void set_window_icon(const std::vector<std::string> &paths);
  void set_window_icon(const std::string &path);

  void set_size(int width, int height);
  glm::ivec2 size();
  int w();
  int h();

  void set_size_limits(int min_width, int min_height, int max_width, int max_height);
  void set_aspect_ratio(int numerator, int denominator);

  void set_pos(int x, int y);
  glm::ivec2 pos();
  int x();
  int y();

  void center(int monitor_num = 0);

  void screenshot(const std::string &path);

  glm::mat4 projection();

#if defined(HADES_PLATFORM_WINDOWS)
  void force_light_mode();
  void force_dark_mode();
#endif

private:
  GLFWwindow *glfw_window_{nullptr};
  GladGLContext *ctx_{nullptr};

  std::unique_ptr<gl::Framebuffer> fbo_{nullptr};
  void create_fbo_();

  struct {
    bool borderless{false};
    bool vsync{false};
  } wm_info_;

  void open_(const WCfg &cfg, glm::ivec2 glversion);
  void close_();

  GLFWmonitor *get_monitor_(int monitor_num);
  void open_fullscreen_windows_(const WCfg &cfg);
  void open_fullscreen_linux_(const WCfg &cfg);
  void open_windowed_(const WCfg &cfg);

#if defined(HADES_PLATFORM_WINDOWS)
  HWND win32_hwnd_{nullptr};
  WNDPROC win32_saved_WndProc_{nullptr};
  bool win32_force_light_mode_{false};
  bool win32_force_dark_mode_{false};

  static void set_win32_titlebar_color_(HWND hwnd);
  static LRESULT CALLBACK WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  void restore_saved_win32_WndProc_();
#endif
};

} // namespace hades

ENABLE_BITMASK_OPERATORS(hades::WFlags)

#endif //HADES_WINDOW_HPP
