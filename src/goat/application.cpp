#include "goat/application.hpp"

namespace goat {

Application::Application() = default;
Application::~Application() = default;

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::initialize_() {
  input = std::make_unique<InputMgr>();
  imgui = std::make_unique<Dear>(window->glfw_handle());

  initialize();
}

void Application::update_(double dt) {
  update(dt);
}

void Application::draw_() {
  imgui->new_frame();

  draw();

  imgui->render();

  window->swap_buffers();
}

void Application::update_event_mgrs_(double dt) {
  input->update_(dt);
}

void Application::open_(const ECfg &engine_create_cfg, const WCfg &cfg) {
  window = std::make_unique<Window>(engine_create_cfg, cfg);
}

} // namespace goat
