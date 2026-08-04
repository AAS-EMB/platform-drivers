cmake_minimum_required(VERSION 3.20)

file(READ VERSION SDK_VERSION)
string(STRIP "${SDK_VERSION}" SDK_VERSION)

project(platform-sdk VERSION ${SDK_VERSION} LANGUAGES C CXX ASM)

option(BUILD_TESTS "Build host unit tests" OFF)
set(PLATFORM "host" CACHE STRING "Target platform: host|stm32f1|stm32f3|stm32f7")
set_property(CACHE PLATFORM PROPERTY STRINGS host stm32f1 stm32f3 stm32f7)
option(SDK_STANDALONE "Build all SDK libraries and embedded compile checks" OFF)
option(CHRONO_CLOCKS_LIB "Build the chrono clocks library" OFF)
option(GPIO_LIB "Build the GPIO library" OFF)
option(CORO_LIB "Build the coroutine library" OFF)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(SDK_STANDALONE ON)
    set(CHRONO_CLOCKS_LIB ON)
    set(GPIO_LIB ON)
    set(CORO_LIB ON)
endif()

set(_sdk_platforms host stm32f1 stm32f3 stm32f7)
if(NOT PLATFORM IN_LIST _sdk_platforms)
    message(FATAL_ERROR "Invalid PLATFORM='${PLATFORM}'. Expected: ${_sdk_platforms}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(sdk_require_file relative_path)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${relative_path}")
        message(FATAL_ERROR
            "Required submodule content is missing: ${relative_path}\n"
            "Initialize only the dependencies for this target with:\n"
            "  ./scripts/init-submodules.sh --platform ${PLATFORM}\n"
            "Add --with-tests when configuring host tests.")
    endif()
endfunction()

add_library(platform_flags INTERFACE)
add_library(platform INTERFACE)
target_link_libraries(platform INTERFACE platform_flags)

set(SDK_BUILD "$ENV{BUILD_NUMBER}")
if(NOT SDK_BUILD)
    set(SDK_BUILD 0)
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE SDK_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT SDK_GIT_HASH)
    set(SDK_GIT_HASH "unknown")
endif()

set(SDK_VERSION_FULL "${PROJECT_VERSION}+build.${SDK_BUILD}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version/sdk_version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/sdk_version.hpp"
    @ONLY
)
target_include_directories(platform_flags INTERFACE
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/${PLATFORM}.cmake")

if(BUILD_TESTS)
    if(NOT PLATFORM STREQUAL "host")
        message(FATAL_ERROR "BUILD_TESTS is supported only with PLATFORM=host")
    endif()
    sdk_require_file("external/googletest/CMakeLists.txt")
    enable_testing()
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/googletest EXCLUDE_FROM_ALL)
endif()

if(CHRONO_CLOCKS_LIB)
    add_subdirectory(chrono_clocks)
endif()
if(GPIO_LIB)
    add_subdirectory(gpio)
endif()
if(CORO_LIB)
    add_subdirectory(coro)
endif()
