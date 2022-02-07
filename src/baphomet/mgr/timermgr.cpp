#include "baphomet/mgr/timermgr.hpp"

#include "baphomet/util/random.hpp"

namespace baphomet {

std::string TimerMgr::after(const std::string &tag, Duration delay, AfterFunc &func) {
  timers_[tag] = std::make_unique<AfterTimer>(delay, func);
  return tag;
}

std::string TimerMgr::after(Duration delay, AfterFunc &func) {
  return after(rnd::base58(11), delay, func);
}

std::string TimerMgr::every(const std::string &tag, Duration interval, EveryFunc &func) {
  timers_[tag] = std::make_unique<EveryTimer>(interval, func);
  return tag;
}

std::string TimerMgr::every(Duration interval, EveryFunc &func) {
  return every(rnd::base58(11), interval, func);
}

std::string TimerMgr::until(const std::string &tag, Duration interval, UntilFunc &func) {
  timers_[tag] = std::make_unique<UntilTimer>(interval, func);
  return tag;
}

std::string TimerMgr::until(Duration interval, UntilFunc &func) {
  return until(rnd::base58(11), interval, func);
}

void TimerMgr::update(Duration dt) {
  for (auto it = timers_.begin(); it != timers_.end();) {
    if (!it->second->paused)
      if (it->second->update(dt))
        it->second->fire();

    if (it->second->expired)
      timers_.erase(it++);
    else
      ++it;
  }
}

void TimerMgr::pause(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = true;
}

void TimerMgr::resume(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = false;
}

void TimerMgr::toggle(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = !it->second->paused;
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

void TimerMgr::AfterTimer::fire() {
  func();
  expired = true;
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

void TimerMgr::EveryTimer::fire() {
  // Potentially execute multiple times if there was a delay
  while (acc_ >= interval_) {
    func();
    acc_ -= interval_;
  }
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

void TimerMgr::UntilTimer::fire() {
  bool ret;

  // Potentially execute multiple times if there was a delay
  while (acc_ >= interval_) {
    ret = func();
    if (!ret)
      break;
    acc_ -= interval_;
  }

  expired = !ret;
}

} // namespace baphomet
