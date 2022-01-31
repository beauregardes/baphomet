#pragma once

#include "baphomet/baphomet.hpp"

#include <fmt/format.h>

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace baphomet {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;

class Ticker {
public:
  Ticker(Duration interval = Duration(0.0));

  bool tick();

  Duration total_elapsed_dt() const;
  Duration dt() const;

private:
  Clock::time_point start_time_;
  Clock::time_point last_time_;

  Duration interval_;
  Duration acc_{0.0};
  Duration dt_{0.0};
};

Ticker::Ticker(Duration interval)
    : start_time_(Clock::now()), last_time_(Clock::now()), interval_(interval) {}

bool Ticker::tick() {
  auto now = Clock::now();
  dt_ = now - last_time_;
  last_time_ = now;

  if (interval_ > Duration(0.0)) {
    acc_ += dt_;
    if (acc_ > interval_) {
      while (acc_ > interval_)
        acc_ -= interval_;
      return true;
    }
  }
  return false;
}

Duration Ticker::total_elapsed_dt() const {
  return last_time_ - start_time_;
}

Duration Ticker::dt() const {
  return dt_;
}

using AfterFunc = const std::function<void(void)>;
using EveryFunc = const std::function<void(void)>;
using UntilFunc = const std::function<bool(void)>;

class TimerMgr {
public:
  std::string after(const std::string &tag, Duration delay, AfterFunc &func);

  std::string every(const std::string &tag, Duration interval, EveryFunc &func);

  std::string until(const std::string &tag, Duration interval, UntilFunc &func);

  void update(Duration dt);

private:
  class Timer_ {
  public:
    explicit Timer_(Duration interval);

    virtual bool update(Duration dt) = 0;
    virtual bool fire() = 0;

  protected:
    Duration interval_;
    Duration acc_{0.0};
  };

  class AfterTimer : public Timer_ {
  public:
    AfterTimer(Duration delay, AfterFunc &func);

    bool update(Duration dt) override;
    bool fire() override;

  private:
    AfterFunc func;
  };

  class EveryTimer : public Timer_ {
  public:
    EveryTimer(Duration interval, EveryFunc &func);

    bool update(Duration dt) override;
    bool fire() override;

  private:
    EveryFunc func;
  };

  class UntilTimer : public Timer_ {
  public:
    UntilTimer(Duration interval, UntilFunc &func);

    bool update(Duration dt) override;
    bool fire() override;

  private:
    UntilFunc func;
  };

  std::map<std::string, std::unique_ptr<Timer_>> timers_;
};

std::string TimerMgr::after(const std::string &tag, Duration delay, AfterFunc &func) {
  timers_[tag] = std::make_unique<AfterTimer>(delay, func);
  return tag;
}

std::string TimerMgr::every(const std::string &tag, Duration interval, EveryFunc &func) {
  timers_[tag] = std::make_unique<EveryTimer>(interval, func);
  return tag;
}

std::string TimerMgr::until(const std::string &tag, Duration interval, UntilFunc &func) {
  timers_[tag] = std::make_unique<UntilTimer>(interval, func);
  return tag;
}

void TimerMgr::update(Duration dt) {
  for (auto it = timers_.begin(); it != timers_.end(); ) {
    if (it->second->update(dt)) {
      if (it->second->fire()) {
        timers_.erase(it++);
        continue;
      }
    }
    ++it;
  }
}

/*********
 * TIMER *
 *********/

TimerMgr::Timer_::Timer_(Duration interval)
    : interval_(interval) {}

/***************
 * AFTER TIMER *
 ***************/

TimerMgr::AfterTimer::AfterTimer(Duration delay, AfterFunc &func)
    : Timer_(delay), func(func) {}

bool TimerMgr::AfterTimer::update(Duration dt) {
  acc_ += dt;
  return acc_ >= interval_;
}

bool TimerMgr::AfterTimer::fire() {
  func();
  return true;
}

/***************
 * EVERY TIMER *
 ***************/

TimerMgr::EveryTimer::EveryTimer(Duration interval, EveryFunc &func)
    : Timer_(interval), func(func) {}

bool TimerMgr::EveryTimer::update(Duration dt) {
  acc_ += dt;
  return acc_ >= interval_;
}

bool TimerMgr::EveryTimer::fire() {
  // Potentially execute multiple times if there was a delay
  while (acc_ >= interval_) {
    func();
    acc_ -= interval_;
  }

  return false;
}

/***************
 * UNTIL TIMER *
 ***************/

TimerMgr::UntilTimer::UntilTimer(Duration interval, UntilFunc &func)
    : Timer_(interval), func(func) {}

bool TimerMgr::UntilTimer::update(Duration dt) {
  acc_ += dt;
  return acc_ >= interval_;
}

bool TimerMgr::UntilTimer::fire() {
  bool ret;

  // Potentially execute multiple times if there was a delay
  while (acc_ >= interval_) {
    ret = func();
    if (!ret)
      break;
    acc_ -= interval_;
  }

  return !ret;
}

} // namespace baphomet