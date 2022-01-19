#include "hades/window.hpp"

#include "hades/util/platform.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if defined(HADES_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <dwmapi.h>
#endif

namespace hades {

Window::~Window() {
#if defined(HADES_PLATFORM_WINDOWS)
  restore_saved_win32_WndProc_();
#endif
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

void Window::set_vsync(bool vsync) {
  wm_info_.vsync = vsync;
  glfwSwapInterval(wm_info_.vsync ? 1 : 0);
}

bool Window::vsync() {
  return wm_info_.vsync;
}

void Window::set_opacity(float opacity) {
  glfwSetWindowOpacity(glfw_window_, opacity);
}

float Window::opacity() {
  return glfwGetWindowOpacity(glfw_window_);
}

void Window::set_window_icon(const std::vector<std::string> &paths) {
  std::vector<GLFWimage> images{};
  for (const auto &p : paths) {
    images.emplace_back();
    images.back().pixels = stbi_load(p.c_str(), &images.back().width, &images.back().height, 0, 4);
  }
  glfwSetWindowIcon(glfw_window_, images.size(), &images[0]);

  for (auto &i : images)
    stbi_image_free(i.pixels);
}

void Window::set_window_icon(const std::string &path) {
  std::vector<std::string> paths = {path};
  set_window_icon(paths);
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

void Window::screenshot(const std::string &path) {
  auto window_size = size();

  ctx_->BindFramebuffer(GL_FRAMEBUFFER, 0);

  auto pixels = std::vector<std::uint8_t>(window_size.x * (4 * window_size.y));
  ctx_->ReadPixels(
      0, 0,
      window_size.x, window_size.y,
      GL_RGBA, GL_UNSIGNED_BYTE,
      (void *)(&pixels[0])
  );

  stbi_flip_vertically_on_write(true);
  stbi_write_png(
      path.c_str(),
      window_size.x, window_size.y, 4,
      &pixels[0],
      window_size.x * 4
  );
}

glm::mat4 Window::projection() {
  auto s = size();
  return glm::ortho(
      0.0f, static_cast<float>(s.x),
      static_cast<float>(s.y), 0.0f,
      0.0f, 1.0f
  );
}

#if defined(HADES_PLATFORM_WINDOWS)
void Window::force_light_mode() {
  win32_force_light_mode_ = true;
  win32_force_dark_mode_ = false;
  set_win32_titlebar_color_(win32_hwnd_);
}

void Window::force_dark_mode() {
  win32_force_light_mode_ = false;
  win32_force_dark_mode_ = true;
  set_win32_titlebar_color_(win32_hwnd_);
}
#endif

void Window::create_fbo_() {
  fbo_ = gl::FramebufferBuilder(ctx_, w(), h())
      .renderbuffer(gl::RBufFormat::rgba8)
      .renderbuffer(gl::RBufFormat::d32f)
      .check_complete();
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

  set_window_icon({
      (RESOURCE_PATH / "icon" / "16x16.png").string(),
      (RESOURCE_PATH / "icon" / "32x32.png").string(),
      (RESOURCE_PATH / "icon" / "48x48.png").string(),
  });

#if defined(HADES_PLATFORM_WINDOWS)
  win32_hwnd_ = glfwGetWin32Window(glfw_window_);
  win32_saved_WndProc_ = (WNDPROC)GetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC);
  win32_force_light_mode_ = cfg.win32_force_light_mode;
  win32_force_dark_mode_ = cfg.win32_force_dark_mode;

  // Set up our Win32 pointers for callbacks
  SetWindowLongPtr(win32_hwnd_, GWLP_USERDATA, (LONG_PTR)this);
  SetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC, (LONG_PTR)WndProc_);

//  if (win32_force_dark_mode_ || win32_force_light_mode_)
  set_win32_titlebar_color_(win32_hwnd_);
#endif

  if (!set(cfg.flags, WFlags::fullscreen) && !set(cfg.flags, WFlags::borderless) && !set(cfg.flags, WFlags::hidden))
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

void Window::open_fullscreen_windows_(const WCfg &cfg) {
  GLFWmonitor *monitor = get_monitor_(cfg.monitor);
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

/*******************************************************************************
 * This code is specifically for setting the titlebar to the dark mode in
 * Windows. It is based on some currently undocumented behavior including
 * a magic constant (I gave it a name that seems to be common online). This
 * does seem to be consistent and stable, so I'm okay relying on it for now
 * even though using behavior that is totally undocumented is a little sketch
 */
#if defined(HADES_PLATFORM_WINDOWS)
void Window::set_win32_titlebar_color_(HWND hwnd) {
  const int DWM_USE_IMMERSIVE_DARK_MODE = 20;
  const BOOL use_light_mode = FALSE;
  const BOOL use_dark_mode = TRUE;

  auto hades_window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  DWORD should_use_light_theme{};
  DWORD should_use_light_theme_size = sizeof(should_use_light_theme);
  LONG code = RegGetValue(
      HKEY_CURRENT_USER,
      R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
      "AppsUseLightTheme",
      RRF_RT_REG_DWORD,
      nullptr,
      &should_use_light_theme,
      &should_use_light_theme_size
  );

  if (code != ERROR_SUCCESS)
    throw std::runtime_error(fmt::format("Cannot read DWORD from registry. {}", code));

  // Incredibly cursed undocumented Win32 API bullshit
  // https://stackoverflow.com/questions/57124243/winforms-dark-title-bar-on-windows-10
  if ((should_use_light_theme || hades_window->win32_force_light_mode_) && !hades_window->win32_force_dark_mode_) {
    const bool ok = SUCCEEDED(DwmSetWindowAttribute(hwnd, DWM_USE_IMMERSIVE_DARK_MODE, &use_light_mode, sizeof(use_light_mode)));
    if (!ok)
      spdlog::warn("Failed to set win32 titlebar to light mode");

  } else if ((!should_use_light_theme || hades_window->win32_force_dark_mode_) && !hades_window->win32_force_light_mode_) {
    const bool ok = SUCCEEDED(DwmSetWindowAttribute(hwnd, DWM_USE_IMMERSIVE_DARK_MODE, &use_dark_mode, sizeof(use_dark_mode)));
    if (!ok)
      spdlog::warn("Failed to set win32 titlebar to dark mode");
  }

  // This is a hack to force Windows to redraw the frame of the window--otherwise it won't
  // specifically when the user forces it without a system-wide change...
  //
  // Could be a problem on systems that don't support window opacity?
  //
  // There is a "solution" that is supposed to use:
  //   SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_DRAWFRAME|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER)
  // But it has not worked for me at all on Windows 10 21H1
  float saved_opacity = hades_window->opacity();
  hades_window->set_opacity(saved_opacity - 0.000001f);
  hades_window->set_opacity(saved_opacity);
}

LRESULT CALLBACK Window::WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  auto hades_window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  if (message == WM_SETTINGCHANGE && hwnd == hades_window->win32_hwnd_)
    set_win32_titlebar_color_(hwnd);

  return CallWindowProc(hades_window->win32_saved_WndProc_, hwnd, message, wParam, lParam);
}

void Window::restore_saved_win32_WndProc_() {
  SetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC, (LONG_PTR)win32_saved_WndProc_);
}

#endif

} // namespace hades