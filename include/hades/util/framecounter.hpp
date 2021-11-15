#ifndef HADES_UTIL_FPS_INFO_HPP
#define HADES_UTIL_FPS_INFO_HPP

#include "hades/util/averagers.hpp"
#include "hades/util/ticker.hpp"

#include <cstdint>
#include <deque>

namespace hades {

class FrameCounter {
public:
    void update();

    double fps();

private:
    std::deque<std::uint64_t> timestamps_{};

    hades::Ticker ticker_{0.5};
    hades::EMA averager_{1.0};
};

} // namespace hades

#endif //HADES_UTIL_FPS_INFO_HPP
