#pragma once

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/util/time/time.hpp"
#include "baphomet/util/random.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <numbers>
#include <string>
#include <unordered_map>

namespace baphomet {

enum class Easing {
  Linear,

  InSine,
  OutSine,
  InOutSine,

  InQuad,
  OutQuad,
  InOutQuad,

  InCubic,
  OutCubic,
  InOutCubic,

  InQuart,
  OutQuart,
  InOutQuart,

  InQuint,
  OutQuint,
  InOutQuint,

  InExp,
  OutExp,
  InOutExp,

  InCirc,
  OutCirc,
  InOutCirc,

  InBack,
  OutBack,
  InOutBack,

  InElastic,
  OutElastic,
  InOutElastic,

  InBounce,
  OutBounce,
  InOutBounce
};

class TweenMgr : Endpoint {

public:
  TweenMgr(std::shared_ptr<Messenger> messenger);

  template <typename T, typename U, typename V>
  requires std::convertible_to<U, T> && std::convertible_to<V, T>
  std::string tween(T &val, U start, V end, Duration duration, Easing easing) {
    return tween(rnd::base58(11), val, start, end, duration, easing);
  }

  template <typename T, typename U, typename V>
  requires std::convertible_to<U, T> && std::convertible_to<V, T>
  std::string tween(std::string const &tag, T &val, U start, V end, Duration duration, Easing easing) {
    tweens_[tag] = std::make_unique<Tween_<T, U, V>>(val, start, end, duration, easing);
    return tag;
  }

  void pause(const std::string &tag);
  void resume(const std::string &tag);
  void toggle(const std::string &tag);

private:
  void received_msg(const MsgCategory &category, const std::any &payload) override;

  void update_(Duration dt);

  class TweenI_ {
  protected:
    static constexpr double c1 = 1.70158;
    static constexpr double c2 = c1 * 1.525;
    static constexpr double c3 = c1 + 1.0;
    static constexpr double c4 = (2.0 * std::numbers::pi) / 3.0;
    static constexpr double c5 = (2.0 * std::numbers::pi) / 4.5;

    static constexpr double n1 = 7.5625;
    static constexpr double d1 = 2.75;

    static std::unordered_map<Easing, std::function<double(double)>> easing_funcs_;
    Easing easing_;

    Duration duration_{0};
    Duration acc_{0};

  public:
    bool paused;

    TweenI_(Duration duration, Easing easing)
        : duration_(duration), easing_(easing), paused(false) {}

    virtual ~TweenI_() = default;

    virtual bool advance(Duration dt) = 0;
  };

  template<typename T, typename U, typename V>
  requires std::convertible_to<U, T> && std::convertible_to<V, T>
  class Tween_ : public TweenI_ {
  private:
    T &val_;
    T start_;
    T end_;

  public:
    Tween_(T &val, U start, V end, Duration duration, Easing easing)
        : TweenI_(duration, easing), val_(val), start_(static_cast<T>(start)), end_(static_cast<T>(end)) {
      val = start;
    }

    bool advance(Duration dt) override {
      acc_ += dt;
      double progress = acc_ / duration_;

      if (progress < 1.0) {
        val_ = start_ + static_cast<T>((end_ - start_) * easing_funcs_[easing_](progress));
        return false;
      } else {
        val_ = end_;
        return true;
      }
    }
  };

  std::unordered_map<std::string, std::unique_ptr<TweenI_>> tweens_{};
};

} // namespace baphomet
