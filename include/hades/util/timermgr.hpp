#ifndef HADES_UTIL_TIMERMGR_HPP
#define HADES_UTIL_TIMERMGR_HPP

#include "hades/util/random.hpp"

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hades {

template<typename T>
concept AfterCallable = requires(T t) { t(); };

template<typename T>
concept EveryCallable = requires(T t) { t(); };

template<typename T>
concept UntilCallable = requires(T t) {
  { t() } -> std::convertible_to<bool>;
};

class TimerMgr {
public:
  TimerMgr() = default;

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  template<AfterCallable Func>
  std::string after(const std::string &tag, const std::vector<double> &intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new AfterTimer(intervals, action));
    return tag;
  }

  template<AfterCallable Func>
  std::string after(const std::string &tag, std::initializer_list<double> intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new AfterTimer(intervals, action));
    return tag;
  }

  template<AfterCallable Func>
  std::string after(const std::string &tag, double interval, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new AfterTimer(std::vector<double>{interval}, action));
    return tag;
  }

  template<AfterCallable Func>
  std::string after(const std::vector<double> &intervals, Func action) {
    auto tag = rnd::base58(11);
    return after(tag, intervals, action);
  }

  template<AfterCallable Func>
  std::string after(std::initializer_list<double> intervals, Func action) {
    auto tag = rnd::base58(11);
    return after(tag, intervals, action);
  }

  template<AfterCallable Func>
  std::string after(double interval, Func action) {
    auto tag = rnd::base58(11);
    return after(tag, interval, action);
  }

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  template<EveryCallable Func>
  std::string every(const std::string &tag, const std::vector<double> &intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryTimer(intervals, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::string &tag, std::initializer_list<double> intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryTimer(intervals, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::string &tag, double interval, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryTimer(std::vector<double>{interval}, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::vector<double> &intervals, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, intervals, action);
  }

  template<EveryCallable Func>
  std::string every(std::initializer_list<double> intervals, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, intervals, action);
  }

  template<EveryCallable Func>
  std::string every(double interval, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, interval, action);
  }

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  template<EveryCallable Func>
  std::string every(const std::string &tag, const std::vector<double> &intervals, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryCountTimer(intervals, count, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::string &tag, std::initializer_list<double> intervals, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryCountTimer(intervals, count, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::string &tag, double interval, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new EveryCountTimer(std::vector<double>{interval}, count, action));
    return tag;
  }

  template<EveryCallable Func>
  std::string every(const std::vector<double> &intervals, int count, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, intervals, count, action);
  }

  template<EveryCallable Func>
  std::string every(std::initializer_list<double> intervals, int count, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, intervals, count, action);
  }

  template<EveryCallable Func>
  std::string every(double interval, int count, Func action) {
    auto tag = rnd::base58(11);
    return every(tag, interval, count, action);
  }

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  template<UntilCallable Func>
  std::string until(const std::string &tag, const std::vector<double> &intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilTimer(intervals, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::string &tag, std::initializer_list<double> intervals, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilTimer(intervals, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::string &tag, double interval, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilTimer(std::vector<double>{interval}, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::vector<double> &intervals, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, intervals, action);
  }

  template<UntilCallable Func>
  std::string until(std::initializer_list<double> intervals, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, intervals, action);
  }

  template<UntilCallable Func>
  std::string until(double interval, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, interval, action);
  }

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  template<UntilCallable Func>
  std::string until(const std::string &tag, const std::vector<double> &intervals, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilCountTimer(intervals, count, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::string &tag, std::initializer_list<double> intervals, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilCountTimer(intervals, count, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::string &tag, double interval, int count, Func action) {
    timers_[tag] = std::unique_ptr<Timer>(new UntilCountTimer(std::vector<double>{interval}, count, action));
    return tag;
  }

  template<UntilCallable Func>
  std::string until(const std::vector<double> &intervals, int count, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, intervals, count, action);
  }

  template<UntilCallable Func>
  std::string until(std::initializer_list<double> intervals, int count, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, intervals, count, action);
  }

  template<UntilCallable Func>
  std::string until(double interval, int count, Func action) {
    auto tag = rnd::base58(11);
    return until(tag, interval, count, action);
  }

  // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

  void update(double dt);

  void toggle(const std::string &tag);

  void pause(const std::string &tag);

  void resume(const std::string &tag);

  void cancel(const std::string &tag);

  void cancel_all();

private:
  class Timer {
  protected:
    double interval_{0.0};
    double acc_{0.0};

  public:
    bool triggered{false};
    bool expired{false};
    bool paused{false};

    Timer(double interval)
      : interval_(interval) {}

    virtual void update(double dt) = 0;

    virtual void fire() = 0;
  };

  template<AfterCallable Func>
  class AfterTimer : public Timer {
  public:
    AfterTimer(const std::vector<double> &intervals, Func action)
      : Timer(rnd::choose(intervals)), action_(action) {}

    void update(double dt) override {
      acc_ += dt;
      if (acc_ >= interval_)
        triggered = true;
    }

    void fire() override {
      action_();

      expired = true;
    }

  private:
    Func action_;
  };

  template<EveryCallable Func>
  class EveryTimer : public Timer {
  public:
    EveryTimer(const std::vector<double> &intervals, Func action)
      : Timer(rnd::choose(intervals)), action_(action), intervals_(intervals) {}

    void update(double dt) override {
      acc_ += dt;
      if (acc_ >= interval_)
        triggered = true;
    }

    void fire() override {
      action_();

      acc_ -= interval_;
      interval_ = rnd::choose(intervals_);

      triggered = false;
    }

  private:
    Func action_;
    std::vector<double> intervals_;
  };

  template<EveryCallable Func>
  class EveryCountTimer : public Timer {
  public:
    EveryCountTimer(const std::vector<double> &intervals, int count, Func action)
      : Timer(rnd::choose(intervals)), action_(action), intervals_(intervals), count_(count) {}

    void update(double dt) override {
      acc_ += dt;
      if (acc_ >= interval_)
        triggered = true;
    }

    void fire() override {
      action_();

      acc_ -= interval_;
      interval_ = rnd::choose(intervals_);

      count_--;
      if (count_ <= 0)
        expired = true;

      triggered = false;
    }

  private:
    Func action_;
    std::vector<double> intervals_;
    int count_;
  };

  template<UntilCallable Func>
  class UntilTimer : public Timer {
  public:
    UntilTimer(const std::vector<double> &intervals, Func action)
      : Timer(rnd::choose(intervals)), action_(action), intervals_(intervals) {}

    void update(double dt) override {
      acc_ += dt;
      if (acc_ >= interval_)
        triggered = true;
    }

    void fire() override {
      bool should_continue = action_();

      if (!should_continue)
        expired = true;

      acc_ -= interval_;
      interval_ = rnd::choose(intervals_);

      triggered = false;
    }

  private:
    Func action_;
    std::vector<double> intervals_;
  };

  template<UntilCallable Func>
  class UntilCountTimer : public Timer {
  public:
    UntilCountTimer(const std::vector<double> &intervals, int count, Func action)
      : Timer(rnd::choose(intervals)), action_(action), intervals_(intervals), count_(count) {}

    void update(double dt) override {
      acc_ += dt;
      if (acc_ >= interval_)
        triggered = true;
    }

    void fire() override {
      bool should_continue = action_();

      if (!should_continue)
        expired = true;

      acc_ -= interval_;
      interval_ = rnd::choose(intervals_);

      count_--;
      if (count_ <= 0)
        expired = true;

      triggered = false;
    }

  private:
    Func action_;
    std::vector<double> intervals_;
    int count_;
  };

  std::unordered_map<std::string, std::unique_ptr<Timer>> timers_{};
};

} // namespace hades

#endif //HADES_UTIL_TIMERMGR_HPP
