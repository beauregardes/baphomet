#include "baphomet/app/window.hpp"

#include "baphomet/util/platform.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace baphomet {

Window::Window(std::shared_ptr<Messenger> messenger) : Endpoint() {
  initialize_endpoint(messenger, MsgEndpoint::Window);
}

void Window::set_size(int width, int height) {
  glfwSetWindowSize(glfw_window_, width, height);
}

glm::ivec2 Window::size() const {
  int width, height;
  glfwGetWindowSize(glfw_window_, &width, &height);
  return {width, height};
}

int Window::w() const {
  int w;
  glfwGetWindowSize(glfw_window_, &w, nullptr);
  return w;
}

int Window::h() const {
  int h;
  glfwGetWindowSize(glfw_window_, nullptr, &h);
  return h;
}

void Window::set_pos(int x, int y) {
  glfwSetWindowPos(glfw_window_, x, y);
}

glm::ivec2 Window::pos() const {
  int x, y;
  glfwGetWindowPos(glfw_window_, &x, &y);
  return {x, y};
}

int Window::x() const {
  int x;
  glfwGetWindowPos(glfw_window_, &x, nullptr);
  return x;
}

int Window::y() const {
  int y;
  glfwGetWindowPos(glfw_window_, nullptr, &y);
  return y;
}

void Window::center(int monitor_num) {
  GLFWmonitor *monitor = get_monitor_(monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  glfwSetWindowPos(
      glfw_window_,
      base_x + (mode->width - w()) / 2,
      base_y + (mode->height - h()) / 2
  );
}

void Window::set_vsync(bool vsync) {
  wm_info_.vsync = vsync;
  glfwSwapInterval(wm_info_.vsync ? 1 : 0);
}

bool Window::vsync() const {
  return wm_info_.vsync;
}

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
  if (visible)
    glfwShowWindow(glfw_window_);
  else
    glfwHideWindow(glfw_window_);
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

glm::mat4 Window::projection() const {
  auto s = size();
  return glm::ortho(
      0.0f, static_cast<float>(s.x),
      static_cast<float>(s.y), 0.0f,
      0.0f, 1.0f
  );
}

void Window::received_msg(const MsgCategory &category, const std::any &payload) {
  switch (category) {
    using enum MsgCategory;

    case WindowFocus: {
#if defined(BAPHOMET_PLATFORM_WINDOWS)
      auto p = extract_msg_payload<WindowFocus>(payload);
      if (wm_info_.borderless)
        set_floating(p.focused == 1);
#endif
    }
      break;

    default:
      Endpoint::received_msg(category, payload);
  }
}

void Window::open_for_gl_(const WCfg &cfg, glm::ivec2 glversion) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glversion.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glversion.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (set(cfg.flags, WFlags::fullscreen) || set(cfg.flags, WFlags::borderless))
    open_fullscreen_(cfg);
  else
    open_windowed_(cfg);

  // Send a message so the runner can save the handle and register callbacks now
  send_msg<MsgCategory::RegisterGlfwCallbacks>(MsgEndpoint::Runner, glfw_window_);

  if (!set(cfg.flags, WFlags::fullscreen) &&
      !set(cfg.flags, WFlags::borderless) &&
      !set(cfg.flags, WFlags::hidden))
    glfwShowWindow(glfw_window_);
}

void Window::close_() {
  glfwSetWindowShouldClose(glfw_window_, GLFW_TRUE);
}

GLFWmonitor *Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  auto monitors = glfwGetMonitors(&monitor_count);

  if (monitor_num >= monitor_count) {
    spdlog::warn(
        "Monitor {} out of range (only {} monitors available); defaulting to primary",
        monitor_num,
        monitor_count
    );
    return monitors[0];
  }
  return monitors[monitor_num];
}

void Window::open_fullscreen_(const WCfg &cfg) {
#if defined(BAPHOMET_PLATFORM_WINDOWS)
  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
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
#elif defined(BAPHOMET_PLATFORM_LINUX)
  /* We are making the assumption that the user is running a version of X11
   * that treats *all* fullscreen windows as borderless fullscreen windows.
   * This seems to generally be true for a good majority of systems. This may
   * also just act exactly like a normal fullscreen, there's not really any
   * way to tell ahead of time.
   */

  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
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
#endif
}

void Window::open_windowed_(const WCfg &cfg) {
  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
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
}

} // namespace baphomet