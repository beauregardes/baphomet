#ifndef HADES_UTIL_TICKER_HPP
#define HADES_UTIL_TICKER_HPP

#include <cstdint>

namespace hades {

class Ticker {
public:
    Ticker(double interval = 0.0);

    bool tick();

    double dt_sec() const;
    double dt_usec() const;
    double dt_msec() const;
    double dt_nsec() const;

    double total_sec() const;
    double total_usec() const;
    double total_msec() const;
    double total_nsec() const;

private:
    std::uint64_t start_ {0};
    std::uint64_t last_ {0};

    std::uint64_t dt_ {0};
    std::uint64_t acc_ {0};
    std::uint64_t interval_ {0};
};

} // namespace hades

#endif //HADES_UTIL_TICKER_HPP
