#pragma once

#include <cstdint>
#include <chrono>
#if defined(PLATFORM_STM32F1)
    #include "stm32f1xx.h"
#elif defined(PLATFORM_STM32F3)
    #include "stm32f3xx.h"
#else
    #error Unsupported platform
#endif

namespace driver {

struct chrono_traits {
    static std::chrono::nanoseconds wall_offset{};

    static void init() noexcept {
        init_systick();
    }

    static std::chrono::nanoseconds monotonic_ticks() noexcept {
        using namespace std::chrono;
        uint32_t h1 = 0u, l = 0u, h2 = 0u;

        do {
            h1 = wall_high;
            l  = wall_low;
            h2 = wall_high;
        } while (h1 not_eq h2);

        auto ms = (static_cast<uint64_t>(h1) << 32) | l;

        return duration_cast<nanoseconds>(
            milliseconds{ static_cast<int64_t>(ms) }
        );
    }

    static std::chrono::nanoseconds wall_ticks() noexcept {
        return monotonic_ticks() + wall_offset;
    }

    static void systick_irq() noexcept {
        if (++wall_low == 0u) {
            ++wall_high;
        }
    }

private:
    // ---------- SysTick (wall time) ----------
    static inline volatile uint32_t wall_low  = 0u;
    static inline volatile uint32_t wall_high = 0u;

    static void init_systick() noexcept {
        SysTick->LOAD = SystemCoreClock / 1000u - 1u;
        SysTick->VAL  = 0u;
        SysTick->CTRL =
            SysTick_CTRL_CLKSOURCE_Msk |
            SysTick_CTRL_TICKINT_Msk |
            SysTick_CTRL_ENABLE_Msk;
    }
};

}
