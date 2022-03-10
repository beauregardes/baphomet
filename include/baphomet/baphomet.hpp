#ifndef BAPHOMET_BAPHOMET_HPP
#define BAPHOMET_BAPHOMET_HPP

#include "baphomet/app/application.hpp"
#include "baphomet/app/runner.hpp"

#include "baphomet/gfx/color.hpp"

#include "baphomet/mgr/timermgr.hpp"

#include "baphomet/noise/perlin.hpp"
#include "baphomet/noise/opensimplex.hpp"

#include "baphomet/util/time/ticker.hpp"
#include "baphomet/util/dear.hpp"
#include "baphomet/util/random.hpp"

#ifndef BAPHOMET_NO_CHRONO_LITERALS
using namespace std::chrono_literals;
#endif

#ifndef BAPHOMET_NO_NAMESPACE_FS
namespace fs = std::filesystem;
#endif

#ifndef BAPHOMET_NO_MAIN_MACROS
#define BAPHOMET_GL_MAIN(a, ...) \
  int main(int, char *[]) {      \
    baphomet::Runner()           \
        .open<a>({               \
            __VA_ARGS__          \
        })                       \
        .init_gl()               \
        .start();                \
  }

#define BAPHOMET_GL_MAIN_DEBUG(a, ...)     \
  int main(int, char *[]) {                \
    baphomet::Runner(spdlog::level::debug) \
        .open<a>({                         \
            __VA_ARGS__                    \
        })                                 \
        .init_gl()                         \
        .start();                          \
  }
#endif

#endif //BAPHOMET_BAPHOMET_HPP
