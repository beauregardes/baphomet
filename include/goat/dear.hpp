#pragma once

#include "goat/configs.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#if defined GOAT_BACKEND_GL
#include "imgui_impl_opengl3.h"
#elif defined GOAT_BACKEND_VK
#include "imgui_impl_vulkan.h"
#else
#error "Must define either GOAT_BACKEND_GL or GL_BACKEND_VK"
#endif

namespace goat {

class Dear {
public:
  Dear(GLFWwindow *window);
  ~Dear();

  void new_frame();
  void render();

private:
  ImGuiContext *ctx_{nullptr};
  ImGuiIO *io_{nullptr};

  void init_for_glfw_(GLFWwindow *window);
  void init_for_gl_();
};

} // namespace goat
