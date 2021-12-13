#ifndef HADES_APPLICATION_HPP
#define HADES_APPLICATION_HPP

#include "gl/framebuffer.hpp"

#include "hades/gfxmgr.hpp"
#include "hades/inputmgr.hpp"
#include "hades/window.hpp"

#include "hades/util/timermgr.hpp"

#include <deque>

namespace hades {

struct DebugLogLine {
  bool should_show{true};
  double timeout{2.5};
  int opacity{255};

  std::string msg{};

  DebugLogLine(const std::string &msg) 
    : msg(msg) {}
};

class Application {
  friend class Runner;

public:
  virtual ~Application() = default;

protected:
  std::unique_ptr<Window> window{nullptr};
  std::unique_ptr<InputMgr> input{nullptr};
  std::unique_ptr<GfxMgr> gfx{nullptr};

  std::unique_ptr<TimerMgr> timer{nullptr};

  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

  template <typename S, typename... Args>
  void debug_log(const S &format, Args &&... args);

  void shutdown();

private:
  GladGLContext *ctx_{nullptr};

  FrameCounter frame_counter_{};

  struct {
    std::unique_ptr<hades::CP437> font{nullptr};

    struct {
      std::deque<DebugLogLine> lines{};
    } log;
  } overlay_{};

  void start_frame_();
  void end_frame_();

  void draw_overlay_();
  void draw_debug_log_();

  void draw_overlay_text_with_bg_(glm::vec2 &base_pos, const std::string &text);
  void draw_overlay_skip_line_(glm::vec2 &base_pos);

  void debug_log_(fmt::string_view format, fmt::format_args args);

  /******************
   * INITIALIZATION *
   ******************/

  void open_(const WCfg &cfg, glm::ivec2 glversion);
  void initgl_(glm::ivec2 glversion);

  void create_fbo_();
};

template <typename S, typename... Args>
void Application::debug_log(const S &format, Args &&... args) {
  debug_log_(format, fmt::make_args_checked<Args...>(format, args...));
}

} // namespace hades

#endif //HADES_APPLICATION_HPP
