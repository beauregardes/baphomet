#include "baphomet/mgr/tweenmgr.hpp"

namespace baphomet {

void TweenMgr::update(double dt) {
  for (auto it = tweens_.begin(); it != tweens_.end(); ) {
    auto &tween = it->second;

    if (!tween->paused)
      if (tween->advance(dt)) {
        delete tween;
        it = tweens_.erase(it);
        continue;
      }

    it++;
  }
}

std::unordered_map<Easing, std::function<double(double)>> TweenMgr::TweenI_::easing_funcs_ = {
    {Easing::linear, [](double progress) -> double {
      return progress;
    }},

    {Easing::in_sine, [](double progress) -> double {
      return 1 - std::cos((progress * std::numbers::pi) / 2.0);
    }},
    {Easing::out_sine, [](double progress) -> double {
      return std::sin((progress * std::numbers::pi) / 2.0);
    }},
    {Easing::in_out_sine, [](double progress) -> double {
      return -(std::cos(progress * std::numbers::pi) - 1) / 2.0;
    }},

    {Easing::in_quad, [](double progress) -> double {
      return progress * progress;
    }},
    {Easing::out_quad, [](double progress) -> double {
      return 1 - (1 - progress) * (1 - progress);
    }},
    {Easing::in_out_quad, [](double progress) -> double {
      return progress < 0.5
             ? 2 * progress * progress
             : 1 - std::pow(-2 * progress + 2, 2) / 2;
    }},

    {Easing::in_cubic, [](double progress) -> double {
      return progress * progress * progress;
    }},
    {Easing::out_cubic, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 3);
    }},
    {Easing::in_out_cubic, [](double progress) -> double {
      return progress < 0.5
             ? 4 * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 3) / 2;
    }},

    {Easing::in_quart, [](double progress) -> double {
      return progress * progress * progress * progress;
    }},
    {Easing::out_quart, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 4);
    }},
    {Easing::in_out_quart, [](double progress) -> double {
      return progress < 0.5
             ? 8 * progress * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 4) / 2;
    }},

    {Easing::in_quint, [](double progress) -> double {
      return progress * progress * progress * progress * progress;
    }},
    {Easing::out_quint, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 5);
    }},
    {Easing::in_out_quint, [](double progress) -> double {
      return progress < 0.5
             ? 16 * progress * progress * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 5) / 2;
    }},

    {Easing::in_exp, [](double progress) -> double {
      return progress == 0.0 ? 0.0 : std::pow(2, 10 * progress - 10);
    }},
    {Easing::out_exp, [](double progress) -> double {
      return progress == 1.0 ? 1.0 : 1 - std::pow(2, -10 * progress);
    }},
    {Easing::in_out_exp, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : progress < 0.5
                 ? std::pow(2, 20 * progress - 10) / 2
                 : (2 - std::pow(2, -20 * progress + 10)) / 2;
    }},

    {Easing::in_circ, [](double progress) -> double {
      return 1 - std::sqrt(1 - std::pow(progress, 2));
    }},
    {Easing::out_circ, [](double progress) -> double {
      return std::sqrt(1 - std::pow(progress - 1, 2));
    }},
    {Easing::in_out_circ, [](double progress) -> double {
      return progress < 0.5
             ? (1 - std::sqrt(1 - std::pow(2 * progress, 2))) / 2
             : (std::sqrt(1 - std::pow(-2 * progress + 2, 2)) + 1) / 2;
    }},

    {Easing::in_back, [](double progress) -> double {
      return c3 * progress * progress * progress - c1 * progress * progress;
    }},
    {Easing::out_back, [](double progress) -> double {
      return 1 + c3 * std::pow(progress - 1, 3) + c1 * std::pow(progress - 1, 2);
    }},
    {Easing::in_out_back, [](double progress) -> double {
      return progress < 0.5
             ? (std::pow(2 * progress, 2) * ((c2 + 1) * 2 * progress - c2)) / 2
             : (std::pow(2 * progress - 2, 2) * ((c2 + 1) * (progress * 2 - 2) + c2) + 2) / 2;
    }},

    {Easing::in_elastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : -std::pow(2, 10 * progress - 10) * std::sin((progress * 10 - 10.75) * c4);
    }},
    {Easing::out_elastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : std::pow(2, -10 * progress) * std::sin((progress * 10 - 0.75) * c4) + 1;
    }},
    {Easing::in_out_elastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : progress < 0.5
                 ? -(std::pow(2, 20 * progress - 10) * std::sin((20 * progress - 11.125) * c5)) / 2
                 : (std::pow(2, -20 * progress + 10) * std::sin((20 * progress - 11.125) * c5)) / 2 + 1;
    }},

    {Easing::in_bounce, [](double progress) -> double {
      return 1 - easing_funcs_[Easing::out_bounce](1 - progress);
    }},
    {Easing::out_bounce, [](double progress) -> double {
      if (progress < 1 / d1) {
        return n1 * progress * progress;
      } else if (progress < 2 / d1) {
        progress -= 1.5 / d1;
        return n1 * progress * progress + 0.75;
      } else if (progress < 2.5 / d1) {
        progress -= 2.25 / d1;
        return n1 * progress * progress + 0.9375;
      } else {
        progress -= 2.625 / d1;
        return n1 * progress * progress + 0.984375;
      }
    }},
    {Easing::in_out_bounce, [](double progress) -> double {
      return progress < 0.5
             ? (1 - easing_funcs_[Easing::out_bounce](1 - 2 * progress)) / 2
             : (1 + easing_funcs_[Easing::out_bounce](2 * progress + 1)) / 2;
    }}
};

} // namespace baphomet