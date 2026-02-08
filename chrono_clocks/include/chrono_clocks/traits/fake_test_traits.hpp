#pragma once

#include <chrono>

namespace driver {

struct chrono_traits {
    static inline int64_t ticks = 0;

    static std::chrono::nanoseconds monotonic_ticks() noexcept {
        return std::chrono::nanoseconds{ ticks };
    }

    static std::chrono::nanoseconds wall_ticks() noexcept {
        return std::chrono::nanoseconds{ ticks };
    }

    static void advance(int64_t delta) noexcept {
        ticks += delta;
    }

    static void reset() noexcept {
        ticks = 0;
    }
    
};

}