#ifndef BAPHOMET_BAPHOMET_HPP
#define BAPHOMET_BAPHOMET_HPP

#include "baphomet/app/application.hpp"
#include "baphomet/app/runner.hpp"

#include "baphomet/gfx/color.hpp"

#include "baphomet/mgr/timermgr.hpp"

#include "baphomet/util/time/ticker.hpp"
#include "baphomet/util/random.hpp"

#ifndef BAPHOMET_NO_CHRONO_LITERALS
using namespace std::chrono_literals;
#endif

#ifndef BAPHOMET_NO_NAMESPACE_FS
namespace fs = std::filesystem;
#endif

#endif //BAPHOMET_BAPHOMET_HPP
