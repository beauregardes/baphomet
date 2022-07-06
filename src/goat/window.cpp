#include "goat/window.hpp"
#include "goat/util/platform.hpp"
#include "spdlog/spdlog.h"

namespace goat {

std::once_flag Window::initialized_glfw_;

Window::Window(const ECfg &engine_create_cfg, const WCfg &cfg) {
  std::call_once(initialized_glfw_, [&]{
    if (!glfwInit()) {
      const char *description;
      int code = glfwGetError(&description);
      spdlog::critical("Failed to initialize GLFW:\n* ({}) {}", code, description);
      std::exit(EXIT_FAILURE);
    }
    spdlog::debug("Initialized GLFW");
  });

  open_(engine_create_cfg, cfg);
}

Window::~Window() {
  glfwDestroyWindow(wm_.handle);
  spdlog::debug("Destroyed GLFW window");
}

void Window::set_should_close(bool should_close) {
  glfwSetWindowShouldClose(wm_.handle, should_close ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_size(int w, int h) {
  glfwSetWindowSize(wm_.handle, w, h);
}

void Window::set_w(int w) {
  glfwSetWindowSize(wm_.handle, w, h());
}

void Window::set_h(int h) {
  glfwSetWindowSize(wm_.handle, w(), h);
}

void Window::set_pos(int x, int y) {
  glfwSetWindowPos(wm_.handle, x, y);
}

void Window::set_x(int x) {
  glfwSetWindowPos(wm_.handle, x, y());
}

void Window::set_y(int y) {
  glfwSetWindowPos(wm_.handle, x(), y);
}

void Window::set_vsync(bool vsync) {
  glfwSwapInterval(vsync ? 1 : 0);
  wm_.vsync = vsync;
}

void Window::set_resizable(bool resizable) {
  glfwSetWindowAttrib(wm_.handle, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_visible(bool visible) {
  if (visible)
    glfwShowWindow(wm_.handle);
  else
    glfwHideWindow(wm_.handle);
}

bool Window::should_close() const {
  return glfwWindowShouldClose(wm_.handle) == GLFW_TRUE;
}

glm::ivec2 Window::size() const {
  int w, h;
  glfwGetWindowSize(wm_.handle, &w, &h);
  return {w, h};
}

int Window::w() const {
  int w;
  glfwGetWindowSize(wm_.handle, &w, nullptr);
  return w;
}

int Window::h() const {
  int h;
  glfwGetWindowSize(wm_.handle, nullptr, &h);
  return h;
}

glm::ivec2 Window::pos() const {
  int x, y;
  glfwGetWindowPos(wm_.handle, &x, &y);
  return {x, y};
}

int Window::x() const {
  int x;
  glfwGetWindowPos(wm_.handle, &x, nullptr);
  return x;
}

int Window::y() const {
  int y;
  glfwGetWindowPos(wm_.handle, nullptr, &y);
  return y;
}

bool Window::vsync() const {
  return wm_.vsync;
}

bool Window::resizable() const {
  return glfwGetWindowAttrib(wm_.handle, GLFW_RESIZABLE);
}

bool Window::visible() const {
  return glfwGetWindowAttrib(wm_.handle, GLFW_VISIBLE);
}

GLFWwindow *Window::glfw_handle() const {
  return wm_.handle;
}

void Window::make_context_current() const {
  glfwMakeContextCurrent(wm_.handle);
}

void Window::swap_buffers() const {
  glfwSwapBuffers(wm_.handle);
}

void Window::open_(const ECfg &engine_create_cfg, const WCfg &cfg) {
#if defined GOAT_BACKEND_GL
  open_for_gl_(engine_create_cfg, cfg);
#elif defined GOAT_BACKEND_VK
#error "Backend NYI"
#else
#error "Must define either GOAT_BACKEND_GL or GL_BACKEND_VK"
#endif
}

void Window::open_for_gl_(const ECfg &engine_create_cfg, const WCfg &cfg) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, engine_create_cfg.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, engine_create_cfg.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (set(cfg.flags, WFlags::fullscreen) || set(cfg.flags, WFlags::borderless))
    open_fullscreen_(cfg);
  else
    open_windowed_(cfg);

  if (!set(cfg.flags, WFlags::fullscreen) &&
      !set(cfg.flags, WFlags::borderless) &&
      !set(cfg.flags, WFlags::hidden))
    glfwShowWindow(wm_.handle);
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
  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

#if defined(GOAT_PLATFORM_WINDOWS)
  if (set(cfg.flags, WFlags::borderless)) {
    wm_.borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    wm_.handle = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), nullptr, nullptr);
  } else
    wm_.handle = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), monitor, nullptr);
#elif defined(GOAT_PLATFORM_LINUX)
  if (set(cfg.flags, WFlags::borderless)) {
    wm_.borderless = true;
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }
  wm_.handle = glfwCreateWindow(mode->width, mode->height, cfg.title.c_str(), monitor, nullptr);
#endif

  if (!wm_.handle) {
    const char *description;
    int code = glfwGetError(&description);
    spdlog::critical("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    spdlog::debug("Created GLFW window");

#if defined(GOAT_PLATFORM_WINDOWS)
  if (set(cfg.flags, WFlags::borderless)) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(wm_.handle, base_x, base_y);
  }
#endif
}

void Window::open_windowed_(const WCfg &cfg) {
  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, set(cfg.flags, WFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);

  wm_.handle = glfwCreateWindow(cfg.size.x, cfg.size.y, cfg.title.c_str(), nullptr, nullptr);
  if (!wm_.handle) {
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
        wm_.handle,
        base_x + (mode->width - cfg.size.x) / 2,
        base_y + (mode->height - cfg.size.y) / 2
    );
  else
    glfwSetWindowPos(
        wm_.handle,
        base_x + cfg.position.x,
        base_y + cfg.position.y
    );
}

} // namespace goat
