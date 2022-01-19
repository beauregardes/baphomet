#ifndef HADES_HADES_HPP
#define HADES_HADES_HPP

#include "hades/util/averagers.hpp"
#include "hades/util/random.hpp"
#include "hades/util/ticker.hpp"
#include "hades/util/time.hpp"

#include "hades/color.hpp"
#include "hades/spritesheet.hpp"
#include "hades/texture.hpp"

#include "hades/application.hpp"
#include "hades/runner.hpp"

#ifndef HADES_NO_CHRONO_LITERALS
using namespace std::chrono_literals;
#endif

#ifndef HADES_NO_NAMESPACE_FS
namespace fs = std::filesystem;
#endif

#endif //HADES_HADES_HPP
