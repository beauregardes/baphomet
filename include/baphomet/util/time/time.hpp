#pragma once

#include <chrono>

namespace baphomet {

/**********************************************************************************************************************
 * RATIONALE
 * --------------------------------------------------------------------------------------------------------------------
 * For the purposes of this library (and user creations), using a floating point representation of time in seconds
 * should be fine because of the magnitudes of the times we are working with, as well as the relative precision we
 * actually require in this domain.
 *
 * As far as magnitudes go, game engines are working with events that take place on the level of milliseconds at a
 * time, maybe microseconds. This is at worst 6 decimal places out--which is a reasonable level of precision needed.
 * Yes, floating point errors can happen, but that leads to the next point.
 *
 * The kinds of floating point errors that are going to happen are not going to matter. Let's say you set up a timer
 * to go off after 2 seconds. The chance of the timer update happening at exactly 2 seconds is almost 0 already, so
 * if the internal representation is off by some small amount, it's not going to make a difference anyway. It wasn't
 * going to fire after 2 seconds, so having it fire at 2.00000093991 seconds isn't going to hurt anything.
 * --------------------------------------------------------------------------------------------------------------------
 */

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;
using Duration = std::chrono::duration<double>;

inline Duration sec(double v) {
  return Duration(v);
}

inline Duration msec(double v) {
  return Duration(v * 1e3);
}

inline Duration usec(double v) {
  return Duration(v * 1e6);
}

inline Duration nsec(double v) {
  return Duration(v * 1e9);
}

} // namespace baphomet