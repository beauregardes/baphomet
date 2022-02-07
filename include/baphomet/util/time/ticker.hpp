#pragma once

#include "baphomet/util/time/time.hpp"

namespace baphomet {

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

} // namespace baphomet
