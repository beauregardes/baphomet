#include "hades/window.hpp"

#include "hades/util/platform.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"

namespace hades {

void Window::set_auto_iconify(bool auto_iconify) {
  glfwSetWindowAttrib(glfw_window_, GLFW_AUTO_ICONIFY, auto_iconify ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::auto_iconify() {
  return glfwGetWindowAttrib(glfw_window_, GLFW_AUTO_ICONIFY) == GLFW_TRUE;
}

void Window::set_floating(bool floating) {
  glfwSetWindowAttrib(glfw_window_, GLFW_FLOATING, floating ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::floating() {
  return glfwGetWindowAttrib(glfw_window_, GLFW_FLOATING) == GLFW_TRUE;
}

void Window::set_resizable(bool resizable) {
  glfwSetWindowAttrib(glfw_window_, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::resizable() {
  return glfwGetWindowAttrib(glfw_window_, GLFW_RESIZABLE) == GLFW_TRUE;
}

void Window::set_visible(bool visible) {
  glfwSetWindowAttrib(glfw_window_, GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::visible() {
  return glfwGetWindowAttrib(glfw_window_, GLFW_VISIBLE) == GLFW_TRUE;
}

void Window::set_decorated(bool decorated) {
  glfwSetWindowAttrib(glfw_window_, GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::decorated() {
  return glfwGetWindowAttrib(glfw_window_, GLFW_DECORATED) == GLFW_TRUE;
}

void Window::set_vsync(bool vsync) {
  wm_info_.vsync = vsync;
  glfwSwapInterval(wm_info_.vsync ? 1 : 0);
}

bool Window::vsync() {
  return wm_info_.vsync;
}

void Window::set_size(int width, int height) {
  glfwSetWindowSize(glfw_window_, width, height);
}

glm::ivec2 Window::size() {
  int width, height;
  glfwGetWindowSize(glfw_window_, &width, &height);
  return {width, height};
}

int Window::w() {
  int w;
  glfwGetWindowSize(glfw_window_, &w, nullptr);
  return w;
}

int Window::h() {
  int h;
  glfwGetWindowSize(glfw_window_, nullptr, &h);
  return h;
}

void Window::set_size_limits(int min_width, int min_height, int max_width, int max_height) {
  glfwSetWindowSizeLimits(glfw_window_, min_width, min_height, max_width, max_height);
}

void Window::set_aspect_ratio(int numerator, int denominator) {
  glfwSetWindowAspectRatio(glfw_window_, numerator, denominator);
}

void Window::set_pos(int x, int y) {
  glfwSetWindowPos(glfw_window_, x, y);
}

glm::ivec2 Window::pos() {
  int x, y;
  glfwGetWindowPos(glfw_window_, &x, &y);
  return {x, y};
}

int Window::x() {
  int x;
  glfwGetWindowPos(glfw_window_, &x, nullptr);
  return x;
}

int Window::y() {
  int y;
  glfwGetWindowPos(glfw_window_, nullptr, &y);
  return y;
}

glm::mat4 Window::projection() {
  auto s = size();
  return glm::ortho(
    0.0f, static_cast<float>(s.x),
    static_cast<float>(s.y), 0.0f,
    0.0f, 1.0f
  );
}

void Window::open_(const WCfg &cfg, glm::ivec2 glversion) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glversion.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glversion.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (set(cfg.flags, WFlags::fullscreen) || set(cfg.flags, WFlags::borderless)) {
#if defined(HADES_PLATFORM_WINDOWS)
    open_fullscreen_windows_(cfg);
#elif defined(HADES_PLATFORM_LINUX)
    open_fullscreen_linux_(cfg);
#endif
  } else
    open_windowed_(cfg);
}

void Window::close_() {
  glfwSetWindowShouldClose(glfw_window_, GLFW_TRUE);
}

GLFWmonitor *Window::get_monitor_(const WCfg &cfg) {
  int monitor_count = 0;
  auto monitors = glfwGetMonitors(&monitor_count);

  if (cfg.monitor >= monitor_count) {
    spdlog::warn(
      "Monitor {} out of range (only {} monitors available); defaulting to primary",
      cfg.monitor,
      monitor_count
    );
    return monitors[0];
  }
  return monitors[cfg.monitor];
}

void Window::open_fullscreen_windows_(const WCfg &cfg) {
  GLFWmonitor *monitor = get_monitor_(cfg);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

//  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (set(cfg.flags, WFlags::borderless)) {
    wm_info_.borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    glfw_window_ = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), nullptr, nullptr);
  } else
    glfw_window_ = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), monitor, nullptr);

  if (!glfw_window_) {
    const char *description;
    int code = glfwGetError(&description);
    spdlog::critical("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    spdlog::debug("Created GLFW window");

  if (set(cfg.flags, WFlags::borderless)) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(glfw_window_, base_x, base_y);
  }
}

void Window::open_fullscreen_linux_(const WCfg &cfg) {
  /* We are making the assumption that the user is running a version of X11
   * that treats *all* fullscreen windows as borderless fullscreen windows.
   * This seems to generally be true for a good majority of systems. This may
   * also just act exactly like a normal fullscreen, there's not really any
   * way to tell ahead of time. The trick with an undecorated window that
   * is the same size as the monitor only seems to work on Windows.
   */

  GLFWmonitor *monitor = get_monitor_(cfg);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (set(cfg.flags, WFlags::borderless)) {
    wm_info_.borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  glfw_window_ = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), monitor, nullptr);
  if (!glfw_window_) {
    const char *description;
    int code = glfwGetError(&description);
    spdlog::critical("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    spdlog::debug("Created GLFW window");
}

void Window::open_windowed_(const WCfg &cfg) {
  GLFWmonitor *monitor = get_monitor_(cfg);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, set(cfg.flags, WFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);

  glfw_window_ = glfwCreateWindow(cfg.size.x, cfg.size.y, cfg.title.c_str(), nullptr, nullptr);
  if (!glfw_window_) {
    const char *description;
    int code = glfwGetError(&description);
    spdlog::critical("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    spdlog::debug("Created GLFW window");

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  if (set(cfg.flags, WFlags::centered))
    glfwSetWindowPos(
      glfw_window_,
      base_x + (mode->width - cfg.size.x) / 2,
      base_y + (mode->height - cfg.size.y) / 2
     );
  else
    glfwSetWindowPos(
      glfw_window_,
      base_x + cfg.position.x,
      base_y + cfg.position.y
    );

  if (!set(cfg.flags, WFlags::hidden))
    glfwShowWindow(glfw_window_);
}

} // namespace hades