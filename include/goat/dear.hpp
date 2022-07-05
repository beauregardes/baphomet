#pragma once

#include "goat/configs.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
