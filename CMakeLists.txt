cmake_minimum_required(VERSION 3.20)
project(drivers_stm LANGUAGES C CXX)

option(BUILD_TESTS "Build unit tests" OFF)
option(PLATFORM "Target platform" "host")

# Drivers options
option(CHRONO_CLOCKS_LIB "Chrono clocks library" OFF)
option(GPIO_LIB "Gpio library" OFF)
option(CORO_LIB "Coroutinue library" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CHRONO_CLOCKS_LIB ON)
    set(GPIO_LIB ON)
    set(CORO_LIB ON)
endif()

add_library(platform_config INTERFACE)

# Platform-specific definitions
if (PLATFORM STREQUAL "host")
    target_compile_definitions(platform_config INTERFACE PLATFORM_HOST BUILD_TESTS)
    target_link_libraries(platform_config INTERFACE pthread)
    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_compile_options(-fcoroutines -Wall -Wextra)
    endif ()
elseif (PLATFORM STREQUAL "stm32f1")
    target_compile_definitions(platform_config INTERFACE STM32F1 PLATFORM_STM32F1)

    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_library(stm32f1_cmsis INTERFACE)

        target_include_directories(stm32f1_cmsis INTERFACE
            ${CMAKE_SOURCE_DIR}/external/cmsis/Core/Include
            ${CMAKE_SOURCE_DIR}/external/cmsis-device-f1/Include
        )

        set(COMMON_COMPILE_OPTIONS
            -mcpu=cortex-m3
        )

        set(COMMON_LINK_OPTIONS
            -mcpu=cortex-m3
            -mthumb
            -Wl,--gc-sections
            -Wl,--print-memory-usage
            -Wl,--start-group
            -Wl,--end-group
            -specs=nano.specs
            -specs=nosys.specs
        )

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m3 -mthumb")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINK_OPTIONS}")

        target_compile_definitions(stm32f1_cmsis INTERFACE
            STM32F100xB
            USE_HAL_DRIVER
        )
    endif()
elseif (PLATFORM STREQUAL "stm32f3")
    target_compile_definitions(platform_config INTERFACE STM32F3 PLATFORM_STM32F3)

    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_library(stm32f3_cmsis INTERFACE)

        target_include_directories(stm32f3_cmsis INTERFACE
            ${CMAKE_SOURCE_DIR}/external/cmsis/Core/Include
            ${CMAKE_SOURCE_DIR}/external/cmsis-device-f3/Include
        )

        set(COMMON_COMPILE_OPTIONS
            -mcpu=cortex-m4
        )

        set(COMMON_LINK_OPTIONS
            -mcpu=cortex-m4
            -mthumb
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
            -Wl,--gc-sections
            -Wl,--print-memory-usage
            -Wl,--start-group
            -Wl,--end-group
            -specs=nano.specs
            -specs=nosys.specs
        )

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -mthumb")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINK_OPTIONS}")

        target_compile_definitions(stm32f3_cmsis INTERFACE
            STM32F303xC
            USE_HAL_DRIVER
        )
    endif()
elseif (PLATFORM STREQUAL "stm32f7")
    target_compile_definitions(platform_config INTERFACE STM32F7 PLATFORM_STM32F7)

    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_library(stm32f7_cmsis INTERFACE)

        target_include_directories(stm32f7_cmsis INTERFACE
            ${CMAKE_SOURCE_DIR}/external/cmsis/Core/Include
            ${CMAKE_SOURCE_DIR}/external/cmsis-device-f7/Include
        )

        set(COMMON_COMPILE_OPTIONS
            -mcpu=cortex-m7
        )

        set(COMMON_LINK_OPTIONS
            -mcpu=cortex-m7
            -mthumb
            -mfpu=fpv5-sp-d16
            -mfloat-abi=hard
            -Wl,--gc-sections
            -Wl,--print-memory-usage
            -Wl,--start-group
            -Wl,--end-group
            -specs=nano.specs
            -specs=nosys.specs
        )

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_OPTIONS}")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m7 -mthumb")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINK_OPTIONS}")

        target_compile_definitions(stm32f7_cmsis INTERFACE
            STM32F767xx
            USE_HAL_DRIVER
        )
    endif()
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