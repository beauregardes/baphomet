#include "baphomet/mgr/tweenmgr.hpp"

#include "spdlog/spdlog.h"

namespace baphomet {

TweenMgr::TweenMgr(std::shared_ptr<Messenger> messenger) : Endpoint() {
  initialize_endpoint(messenger, MsgEndpoint::Tween);
}

void TweenMgr::pause(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = true;
}

void TweenMgr::resume(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = false;
}

void TweenMgr::toggle(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = !it->second->paused;
}

void TweenMgr::received_msg(const MsgCategory &category, const std::any &payload) {
  switch (category) {
    using enum MsgCategory;

    case Update: {
      auto p = extract_msg_payload<Update>(payload);
      update_(p.dt);
    }
      break;

    default:
      Endpoint::received_msg(category, payload);
  }
}

void TweenMgr::update_(Duration dt) {
  for (auto it = tweens_.begin(); it != tweens_.end(); ) {
    auto &tween = it->second;

    if (!tween->paused)
      if (tween->advance(dt)) {
        it = tweens_.erase(it);
        continue;
      }

    it++;
  }
}

std::unordered_map<Easing, std::function<double(double)>> TweenMgr::TweenI_::easing_funcs_ = {
    {Easing::Linear, [](double progress) -> double {
      return progress;
    }},

    {Easing::InSine, [](double progress) -> double {
      return 1 - std::cos((progress * std::numbers::pi) / 2.0);
    }},
    {Easing::OutSine, [](double progress) -> double {
      return std::sin((progress * std::numbers::pi) / 2.0);
    }},
    {Easing::InOutSine, [](double progress) -> double {
      return -(std::cos(progress * std::numbers::pi) - 1) / 2.0;
    }},

    {Easing::InQuad, [](double progress) -> double {
      return progress * progress;
    }},
    {Easing::OutQuad, [](double progress) -> double {
      return 1 - (1 - progress) * (1 - progress);
    }},
    {Easing::InOutQuad, [](double progress) -> double {
      return progress < 0.5
             ? 2 * progress * progress
             : 1 - std::pow(-2 * progress + 2, 2) / 2;
    }},

    {Easing::InCubic, [](double progress) -> double {
      return progress * progress * progress;
    }},
    {Easing::OutCubic, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 3);
    }},
    {Easing::InOutCubic, [](double progress) -> double {
      return progress < 0.5
             ? 4 * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 3) / 2;
    }},

    {Easing::InQuart, [](double progress) -> double {
      return progress * progress * progress * progress;
    }},
    {Easing::OutQuart, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 4);
    }},
    {Easing::InOutQuart, [](double progress) -> double {
      return progress < 0.5
             ? 8 * progress * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 4) / 2;
    }},

    {Easing::InQuint, [](double progress) -> double {
      return progress * progress * progress * progress * progress;
    }},
    {Easing::OutQuint, [](double progress) -> double {
      return 1 - std::pow(1 - progress, 5);
    }},
    {Easing::InOutQuint, [](double progress) -> double {
      return progress < 0.5
             ? 16 * progress * progress * progress * progress * progress
             : 1 - std::pow(-2 * progress + 2, 5) / 2;
    }},

    {Easing::InExp, [](double progress) -> double {
      return progress == 0.0 ? 0.0 : std::pow(2, 10 * progress - 10);
    }},
    {Easing::OutExp, [](double progress) -> double {
      return progress == 1.0 ? 1.0 : 1 - std::pow(2, -10 * progress);
    }},
    {Easing::InOutExp, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : progress < 0.5
                 ? std::pow(2, 20 * progress - 10) / 2
                 : (2 - std::pow(2, -20 * progress + 10)) / 2;
    }},

    {Easing::InCirc, [](double progress) -> double {
      return 1 - std::sqrt(1 - std::pow(progress, 2));
    }},
    {Easing::OutCirc, [](double progress) -> double {
      return std::sqrt(1 - std::pow(progress - 1, 2));
    }},
    {Easing::InOutCirc, [](double progress) -> double {
      return progress < 0.5
             ? (1 - std::sqrt(1 - std::pow(2 * progress, 2))) / 2
             : (std::sqrt(1 - std::pow(-2 * progress + 2, 2)) + 1) / 2;
    }},

    {Easing::InBack, [](double progress) -> double {
      return c3 * progress * progress * progress - c1 * progress * progress;
    }},
    {Easing::OutBack, [](double progress) -> double {
      return 1 + c3 * std::pow(progress - 1, 3) + c1 * std::pow(progress - 1, 2);
    }},
    {Easing::InOutBack, [](double progress) -> double {
      return progress < 0.5
             ? (std::pow(2 * progress, 2) * ((c2 + 1) * 2 * progress - c2)) / 2
             : (std::pow(2 * progress - 2, 2) * ((c2 + 1) * (progress * 2 - 2) + c2) + 2) / 2;
    }},

    {Easing::InElastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : -std::pow(2, 10 * progress - 10) * std::sin((progress * 10 - 10.75) * c4);
    }},
    {Easing::OutElastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : std::pow(2, -10 * progress) * std::sin((progress * 10 - 0.75) * c4) + 1;
    }},
    {Easing::InOutElastic, [](double progress) -> double {
      return progress == 0.0
             ? 0.0
             : progress == 1.0
               ? 1.0
               : progress < 0.5
                 ? -(std::pow(2, 20 * progress - 10) * std::sin((20 * progress - 11.125) * c5)) / 2
                 : (std::pow(2, -20 * progress + 10) * std::sin((20 * progress - 11.125) * c5)) / 2 + 1;
    }},

    {Easing::InBounce,      [](double progress) -> double {
      return 1 - easing_funcs_[Easing::OutBounce](1 - progress);
    }},
    {Easing::OutBounce,     [](double progress) -> double {
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
    {Easing::InOutBounce, [](double progress) -> double {
      return progress < 0.5
             ? (1 - easing_funcs_[Easing::OutBounce](1 - 2 * progress)) / 2
             : (1 + easing_funcs_[Easing::OutBounce](2 * progress - 1)) / 2;
    }}
};

} // namespace baphomet