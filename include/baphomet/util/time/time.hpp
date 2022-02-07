#pragma once

#include <chrono>

namespace baphomet {

using Clock = std::chrono::steady_clock;

using Duration = std::chrono::duration<double>;

} // namespace baphomet