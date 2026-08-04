add_library(chrono_clocks INTERFACE)

target_include_directories(chrono_clocks INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(chrono_clocks INTERFACE
    cxx_std_20
)

target_link_libraries(chrono_clocks INTERFACE platform)

if (BUILD_TESTS AND PLATFORM STREQUAL "host")
    add_subdirectory(tests)
endif()

if (NOT PLATFORM STREQUAL "host" AND SDK_STANDALONE)
    add_library(chrono_compile_check OBJECT
        ${CMAKE_CURRENT_LIST_DIR}/tests/compile_check.cpp
    )

    target_link_libraries(chrono_compile_check PRIVATE
        chrono_clocks
    )
endif()
