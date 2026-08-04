add_library(coro_lib INTERFACE)

target_include_directories(coro_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(coro_lib INTERFACE
    cxx_std_20
)

target_link_libraries(coro_lib INTERFACE platform)

if (BUILD_TESTS AND PLATFORM STREQUAL "host")
    add_subdirectory(tests)
endif()

if (NOT PLATFORM STREQUAL "host" AND SDK_STANDALONE)
    add_library(coro_compile_check OBJECT
        ${CMAKE_CURRENT_LIST_DIR}/tests/compile_check.cpp
    )

    target_link_libraries(coro_compile_check PRIVATE
        coro_lib
        chrono_clocks
    )
endif()
