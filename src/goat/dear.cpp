#include "goat/dear.hpp"

#include "spdlog/spdlog.h"
#include <string>

namespace goat {

Dear::Dear(GLFWwindow *window) {
  IMGUI_CHECKVERSION();

  ctx_ = ImGui::CreateContext();
  io_ = &ImGui::GetIO();

  init_for_glfw_(window);

#if defined(GOAT_BACKEND_GL)
  init_for_gl_();
#else
  spdlog::critical("Backend NYI");
  std::exit(EXIT_FAILURE);
#endif
}

Dear::~Dear() {
  ImGui::DestroyContext(ctx_);
}

void Dear::new_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Dear::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Dear::init_for_glfw_(GLFWwindow *window) {
  ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Dear::init_for_gl_() {
//  std::string glsl_version;
//  if      (gl_version == glm::ivec2{2, 0}) glsl_version = "#version 110";
//  else if (gl_version == glm::ivec2{2, 1}) glsl_version = "#version 120";
//  else if (gl_version == glm::ivec2{3, 0}) glsl_version = "#version 130";
//  else if (gl_version == glm::ivec2{3, 1}) glsl_version = "#version 140";
//  else if (gl_version == glm::ivec2{3, 2}) glsl_version = "#version 150";
//  else
//    glsl_version = fmt::format("#version {}{}0", gl_version.x, gl_version.y);
//  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
  ImGui_ImplOpenGL3_Init();
}

} // namespace goat
