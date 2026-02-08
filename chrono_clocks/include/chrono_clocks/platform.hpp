#pragma once

#if defined(PLATFORM_STM32F7)
    #include <chrono_clocks/traits/stm32_dwt_traits.hpp>
#elif defined(PLATFORM_STM32F3) or defined(PLATFORM_STM32F1)
    #include <chrono_clocks/traits/stm32_systick_traits.hpp>
#elif defined(PLATFORM_HOST)
    #if defined(BUILD_TESTS)
        #include <chrono_clocks/traits/fake_test_traits.hpp>
    #else
        #include <chrono_clocks/traits/host_traits.hpp>
    #endif
#else
    #error "Unknown chrono platform"
#endif