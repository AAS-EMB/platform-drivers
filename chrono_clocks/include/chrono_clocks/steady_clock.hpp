#pragma once

#include <chrono>
#include <cstdint>
#include <chrono_clocks/platform.hpp>
#include <chrono_clocks/clock_concept.hpp>

namespace driver {

class steady_clock {
public:
    using traits     = chrono_traits;
    using duration   = std::chrono::nanoseconds;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<steady_clock>;

    static constexpr bool is_steady = true;

    static time_point now() noexcept {
        return time_point{traits::monotonic_ticks()};
    }
};

static_assert(Clock<steady_clock>,
              "steady_clock must satisfy Clock concept");

}