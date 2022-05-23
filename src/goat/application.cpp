#include "goat/application.hpp"

namespace goat {

Application::Application() = default;

Application::~Application() {

}

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::open_(const WCfg &cfg) {
  window = std::make_unique<Window>(cfg);
}

} // namespace goat
