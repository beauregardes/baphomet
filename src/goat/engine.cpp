#include "goat/engine.hpp"
#include "glbinding/glbinding.h"
#include "spdlog/spdlog.h"

namespace goat {

Engine::Engine() = default;

void Engine::init_gfx_backend_(const WCfg &cfg) {
  switch (cfg.gfx_backend) {
    case WBackend::gl:
      init_gl_(cfg);
      break;

    default:
      spdlog::critical("Backend NYI");
      std::exit(EXIT_FAILURE);
  }
}

void Engine::init_gl_(const WCfg &cfg) {
  application_->window->make_context_current();
  glbinding::initialize(glfwGetProcAddress);
}

void Engine::register_glfw_callbacks_() {
  auto handle = application_->window->glfw_handle();
}

void Engine::run_() {
  application_->initialize();

  do {
    application_->update(0.0);
    application_->draw();

    application_->window->swap_buffers();

    glfwPollEvents();
  } while (!application_->window->should_close());
}

} // namespace goat
