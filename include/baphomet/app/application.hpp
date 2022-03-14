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
#include "implot.h"

#include <concepts>
#include <deque>
#include <map>

namespace baphomet {

struct DebugLogLine {
  bool should_show{true};
  Duration timeout{baphomet::sec(2.5)};
  int opacity{255};

  std::string msg{};

  DebugLogLine(const std::string &msg) : msg(msg) {}
};

class Application : Endpoint {
  friend class Runner;

public:
  Application();
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

  void set_overlay_enabled(bool enabled);

  template <typename S, typename... Args>
  void debug_log(const S &format, Args &&... args);

  template <typename F> requires std::invocable<F>
  void sticky(const std::string &label, const std::string &format, F f);

  template <typename F> requires std::invocable<F>
  void sticky(const std::string &label, F f);

  void unsticky(const std::string &label);
  void unsticky_all();

  void shutdown();

private:
  std::shared_ptr<Messenger> messenger_{nullptr};
  void received_msg(const MsgCategory &category, const std::any &payload) override;

  FrameCounter frame_counter_{};

  struct {
    bool enabled{false};

    std::shared_ptr<RenderTarget> render_target{nullptr};

    std::unique_ptr<CP437> font{nullptr};

    struct {
      std::deque<DebugLogLine> lines{};
    } log;
  } overlay_{};

  struct {
    std::shared_ptr<RenderTarget> render_target{nullptr};

    ImGuiContext *ctx{nullptr};
    ImGuiIO *io{nullptr};

    bool newframe_called{false};
  } imgui_state_;

  struct {
    ImPlotContext *ctx{nullptr};
  } implot_state_;

  void imgui_startframe_();
  void imgui_endframe_();

  void update_nonuser_(Duration dt);
  void poll_events_();
  bool should_close_();

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

  void open_for_gl_(std::shared_ptr<Messenger> &messenger, const WCfg &cfg, glm::ivec2 glversion);
  void init_gl_(glm::ivec2 glversion);
  void init_imgui_(glm::ivec2 glversion);

  /************
   * STICKIES *
   ************/

  class StickyI {
  public:
    std::string label;

    explicit StickyI(const std::string &label)
        : label(label) {}

    virtual std::string str() = 0;
  };

  template <typename F>
  class Sticky : public StickyI {
  public:
    Sticky(const std::string &label, const std::string &format, F f)
        : StickyI(label), format_(format), f_(f) {}

    std::string str() override {
      auto args = fmt::make_args_checked<std::invoke_result_t<F>>(format_, f_());
      return fmt::format("{}: {}", label, fmt::vformat(format_, args));
    }

  private:
    std::string format_;
    F f_;
  };

  std::vector<std::unique_ptr<StickyI>> stickies_{};
};

template <typename S, typename... Args>
void Application::debug_log(const S &format, Args &&... args) {
  debug_log_(format, fmt::make_args_checked<Args...>(format, args...));
}

template <typename F> requires std::invocable<F>
void Application::sticky(const std::string &label, const std::string &format, F f) {
  stickies_.emplace_back(std::unique_ptr<StickyI>(new Sticky(label, format, f)));
}

template <typename F> requires std::invocable<F>
void Application::sticky(const std::string &label, F f) {
  sticky(label, "{}", f);
}

} // namespace baphomet
