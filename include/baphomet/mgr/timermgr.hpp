#pragma once

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/util/time/time.hpp"

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace baphomet {

using AfterFunc = const std::function<void(void)>;
using EveryFunc = const std::function<void(void)>;
using UntilFunc = const std::function<bool(void)>;

class TimerMgr : Endpoint {
public:
  TimerMgr(std::shared_ptr<Messenger> messenger);

  std::string after(const std::string &tag, Duration delay, AfterFunc &func);
  std::string after(Duration delay, AfterFunc &func);

  std::string every(const std::string &tag, Duration interval, EveryFunc &func);
  std::string every(Duration interval, EveryFunc &func);

  std::string until(const std::string &tag, Duration interval, UntilFunc &func);
  std::string until(Duration interval, UntilFunc &func);

  void pause(const std::string &tag);
  void resume(const std::string &tag);
  void toggle(const std::string &tag);

private:
  void received_msg(const MsgCategory &category, const std::any &payload) override;

  void update_(Duration dt);

  class Timer_ {
  public:
    bool paused{false};
    bool expired{false};

    explicit Timer_(Duration interval);

    virtual bool update(Duration dt) = 0;
    virtual void fire() = 0;

  protected:
    Duration interval_;
    Duration acc_{0.0};
  };

  class AfterTimer : public Timer_ {
  public:
    AfterTimer(Duration delay, AfterFunc &func);

    bool update(Duration dt) override;
    void fire() override;

  private:
    AfterFunc func;
  };

  class EveryTimer : public Timer_ {
  public:
    EveryTimer(Duration interval, EveryFunc &func);

    bool update(Duration dt) override;
    void fire() override;

  private:
    EveryFunc func;
  };

  class UntilTimer : public Timer_ {
  public:
    UntilTimer(Duration interval, UntilFunc &func);

    bool update(Duration dt) override;
    void fire() override;

  private:
    UntilFunc func;
  };

  std::map<std::string, std::unique_ptr<Timer_>> timers_;
};

} // namespace baphomet