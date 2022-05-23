#pragma once

#include "goat/window.hpp"
#include <memory>

namespace goat {

class Application {
  friend class Engine;

public:
  Application();
  virtual ~Application();

protected:
  std::unique_ptr<Window> window{nullptr};

  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

private:
  void open_(const WCfg &cfg);
};

} // namespace goat

