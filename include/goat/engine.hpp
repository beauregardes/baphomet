#pragma once

#include "goat/application.hpp"
#include <concepts>

namespace goat {

template<typename T>
concept DerivesGoatApplication = std::is_base_of_v<Application, T>;

class Engine {
public:
  Engine();

  template <DerivesGoatApplication T>
  void open(const WCfg &cfg);

private:
  std::unique_ptr<Application> application_{nullptr};

  void init_gfx_backend_(const WCfg &cfg);
  void init_gl_(const WCfg &cfg);

  void register_glfw_callbacks_();

  void run_();
};

template <DerivesGoatApplication T>
void Engine::open(const WCfg &cfg) {
  application_ = std::unique_ptr<Application>(new T());

  application_->open_(cfg);
  register_glfw_callbacks_();

  init_gfx_backend_(cfg);
  run_();
}

} // namespace goat
