#pragma once

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/app/window.hpp"
#include "baphomet/gfx/gl/framebuffer.hpp"
#include "baphomet/mgr/audiomgr.hpp"
#include "baphomet/mgr/gfxmgr.hpp"
#include "baphomet/mgr/inputmgr.hpp"
#include "baphomet/mgr/timermgr.hpp"
#include "baphomet/mgr/tweenmgr.hpp"
#include "baphomet/util/time/time.hpp"
#include "baphomet/util/framecounter.hpp"

#include "imgui.h"

#include <deque>

namespace baphomet {

struct DebugLogLine {
  bool should_show{true};
  Duration timeout{std::chrono::milliseconds(2500)};
  int opacity{255};

  std::string msg{};

  DebugLogLine(const std::string &msg) : msg(msg) {}
};

class Application {
  friend class Runner;

public:
  virtual ~Application();

protected:
  std::unique_ptr<Window> window{nullptr};
  std::unique_ptr<InputMgr> input{nullptr};
  std::unique_ptr<GfxMgr> gfx{nullptr};

  std::unique_ptr<AudioMgr> audio{nullptr};
  std::unique_ptr<TimerMgr> timer{nullptr};
  std::unique_ptr<TweenMgr> tween{nullptr};

  virtual void initialize();
  virtual void update(Duration dt);
  virtual void draw();

  template <typename S, typename... Args>
  void debug_log(const S &format, Args &&... args);

  void shutdown();

private:
  std::shared_ptr<Messenger> msgr_;
  void received_message_(const MsgCat &category, const std::any &payload);

  FrameCounter frame_counter_{};

  struct {
    std::unique_ptr<CP437> font{nullptr};

    struct {
      std::deque<DebugLogLine> lines{};
    } log;
  } overlay_{};

  struct {
    ImGuiContext *ctx{nullptr};
    ImGuiIO *io{nullptr};
    bool newframe_called{false};
  } imgui_state_;

  void imgui_startframe_();
  void imgui_endframe_();

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

  void open_for_gl_(const WCfg &cfg, glm::ivec2 glversion);
  void init_gl_(glm::ivec2 glversion);
  void init_imgui_(glm::ivec2 glversion);
};

template <typename S, typename... Args>
void Application::debug_log(const S &format, Args &&... args) {
  debug_log_(format, fmt::make_args_checked<Args...>(format, args...));
}

} // namespace baphomet
