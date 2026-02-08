#pragma once

#include <chrono>
#include <cstdint>
#include <chrono_clocks/platform.hpp>
#include <chrono_clocks/clock_concept.hpp>

namespace driver {

class system_clock {
public:
    using traits     = chrono_traits;
    using duration   = std::chrono::nanoseconds;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<system_clock>;

    static constexpr bool is_steady = false;

    static time_point now() noexcept {
        return time_point{traits::wall_ticks()}; // utc time
    }
};

static_assert(Clock<system_clock>,
              "system_clock must satisfy Clock concept");

}