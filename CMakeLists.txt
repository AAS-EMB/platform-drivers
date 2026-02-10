cmake_minimum_required(VERSION 3.20)

file(READ VERSION SDK_VERSION)
string(STRIP "${SDK_VERSION}" SDK_VERSION)

project(platform-sdk VERSION ${SDK_VERSION} LANGUAGES C CXX ASM)

# =========================================================
# Options
# =========================================================

option(BUILD_TESTS "Build unit tests" OFF)
set(PLATFORM "host" CACHE STRING "Target platform: host|stm32f1|stm32f3|stm32f7")
if(NOT PLATFORM MATCHES "^(host|stm32f1|stm32f3|stm32f7)$")
    message(FATAL_ERROR "Invalid PLATFORM: ${PLATFORM}")
endif()

option(SDK_STANDALONE "Build SDK standalone (enable compile checks)" OFF)

option(CHRONO_CLOCKS_LIB "Chrono clocks library" OFF)
option(GPIO_LIB "Gpio library" OFF)
option(CORO_LIB "Coroutinue library" OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(SDK_STANDALONE ON)
    set(CHRONO_CLOCKS_LIB ON)
    set(GPIO_LIB ON)
    set(CORO_LIB ON)
endif()

# =========================================================
# Language
# =========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================
# Platform flags (CORE)
# =========================================================

add_library(platform_flags INTERFACE)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/cmake/platforms/${PLATFORM}.cmake)
    message(FATAL_ERROR "Platform file not found: ${PLATFORM}")
endif()

include(cmake/platforms/${PLATFORM}.cmake)

target_compile_options(platform_flags INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${PLATFORM_COMMON_FLAGS} ${PLATFORM_C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${PLATFORM_COMMON_FLAGS} ${PLATFORM_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${PLATFORM_COMMON_FLAGS}>
)

target_link_options(platform_flags INTERFACE
    ${PLATFORM_LINK_COMMON}
    ${PLATFORM_LINK_SCRIPT}
)

target_link_libraries(platform_flags INTERFACE
    ${PLATFORM_LINK_LIBS}
)

# =========================================================
# External dependencies
# =========================================================
if (PLATFORM STREQUAL "host")
    target_compile_definitions(platform_flags INTERFACE
        PLATFORM_HOST
        BUILD_TESTS
    )

    target_link_libraries(platform_flags INTERFACE
        pthread
    )
elseif (PLATFORM STREQUAL "stm32f1")
    include(cmake/external/cmsis/stm32f1.cmake)
    include(cmake/external/hal/stm32f1.cmake)
    target_compile_definitions(platform_flags INTERFACE
        PLATFORM_STM32F1
    )
elseif (PLATFORM STREQUAL "stm32f3")
    include(cmake/external/cmsis/stm32f3.cmake)
    include(cmake/external/hal/stm32f3.cmake)
    target_compile_definitions(platform_flags INTERFACE
        PLATFORM_STM32F3
    )
elseif (PLATFORM STREQUAL "stm32f7")
    include(cmake/external/cmsis/stm32f7.cmake)
    include(cmake/external/hal/stm32f7.cmake)
    target_compile_definitions(platform_flags INTERFACE
        PLATFORM_STM32F7
    )
endif()

if (BUILD_TESTS)
    enable_testing()
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/googletest)
endif()

if (CHRONO_CLOCKS_LIB)
    add_subdirectory(chrono_clocks)
endif()
if (GPIO_LIB)
    add_subdirectory(gpio)
endif()
if (CORO_LIB)
    add_subdirectory(coro)
endif()