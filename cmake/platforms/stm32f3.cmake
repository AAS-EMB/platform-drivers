message(STATUS "Using STM32F3 platform")

set(MCU cortex-m4)

set(PLATFORM_COMMON_FLAGS
    -mcpu=${MCU}
    -mthumb
)

set(PLATFORM_C_FLAGS
    -ffunction-sections
    -fdata-sections
)

set(PLATFORM_CXX_FLAGS
    ${PLATFORM_C_FLAGS}
    -fcoroutines
)

set(PLATFORM_LINK_COMMON
    -mcpu=${MCU}
    -mthumb
    -Wl,--gc-sections
)

set(PLATFORM_LINK_LIBS
    -lc
    -lm
)