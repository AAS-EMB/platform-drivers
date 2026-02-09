#pragma once

#include <cstdint>
#include <chrono>
#if defined(PLATFORM_STM32F7)
    #include "stm32f7xx.h"
#else
    #error Unsupported platform
#endif

namespace driver {

struct chrono_traits {
    static std::chrono::nanoseconds wall_offset{};

    static void init() noexcept {
        init_dwt();
    }

    static std::chrono::nanoseconds monotonic_ticks() noexcept {
        using namespace std::chrono;
        uint32_t h1, l, h2;

        do {
            h1 = dwt_high;
            l  = DWT->CYCCNT;
            h2 = dwt_high;
        } while (h1 not_eq h2);

        const uint64_t cycles = (static_cast<uint64_t>(h1) << 32) | l;

        return nanoseconds{
            static_cast<int64_t>((cycles * 1'000'000'000ull) / SystemCoreClock)
        };
    }

    static std::chrono::nanoseconds wall_ticks() noexcept {
        return monotonic_ticks() + wall_offset;
    }

private:
    // ---------- DWT (monotonic) ----------
    static inline uint32_t dwt_last = 0u;
    static inline uint32_t dwt_high = 0u;

    static void init_dwt() noexcept {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0u;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

};

}