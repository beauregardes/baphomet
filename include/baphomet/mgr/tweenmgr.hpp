#pragma once

#include "baphomet/util/random.hpp"

#include <functional>
#include <numbers>
#include <string>
#include <unordered_map>

namespace baphomet {

enum class Easing {
  linear,
  in_sine,
  out_sine,
  in_out_sine,
  in_quad,
  out_quad,
  in_out_quad,
  in_cubic,
  out_cubic,
  in_out_cubic,
  in_quart,
  out_quart,
  in_out_quart,
  in_quint,
  out_quint,
  in_out_quint,
  in_exp,
  out_exp,
  in_out_exp,
  in_circ,
  out_circ,
  in_out_circ,
  in_back,
  out_back,
  in_out_back,
  in_elastic,
  out_elastic,
  in_out_elastic,
  in_bounce,
  out_bounce,
  in_out_bounce
};

class TweenMgr {

public:
  TweenMgr() = default;
  ~TweenMgr() = default;

  template <typename T>
  std::string tween(T &val, T start, T end, double duration, Easing easing) {
    return tween(rnd::base58(11), val, start, end, duration, easing);
  }

  template <typename T>
  std::string tween(std::string const &tag, T &val, T start, T end, double duration, Easing easing) {
    tweens_[tag] = new Tween_<T>(val, start, end, duration, easing);
    return tag;
  }

  void update(double dt);

private:
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

    double duration_;
    double acc_;

  public:
    bool paused;

    TweenI_(double duration, Easing easing)
        : duration_(duration), acc_(0.0), easing_(easing), paused(false) {}

    virtual ~TweenI_() = default;

    virtual bool advance(double dt) = 0;
  };

  template<typename T>
  class Tween_ : public TweenI_ {
  private:
    T &val_;
    T start_;
    T end_;

  public:
    Tween_(T &val, T start, T end, double duration, Easing easing)
        : TweenI_(duration, easing), val_(val), start_(start), end_(end) {
      val = start;
    }

    bool advance(double dt) override {
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

  std::unordered_map<std::string, TweenI_ *> tweens_{};
};

} // namespace baphomet
