#pragma once

#include "goat/configs.hpp"
#include "goat/dear.hpp"
#include "goat/inputmgr.hpp"
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
  std::unique_ptr<InputMgr> input{nullptr};
  std::unique_ptr<Dear> imgui{nullptr};

  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

private:
  void initialize_();
  void update_(double dt);
  void draw_();
  void update_event_mgrs_(double dt);

  void open_(const ECfg &engine_create_cfg, const WCfg &cfg);
};

} // namespace goat

