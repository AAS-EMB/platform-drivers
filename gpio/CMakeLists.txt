add_library(gpio_driver INTERFACE)

target_include_directories(gpio_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(gpio_driver INTERFACE
    cxx_std_20
)

target_link_libraries(gpio_driver INTERFACE platform)

if (BUILD_TESTS AND PLATFORM STREQUAL "host")
    add_subdirectory(tests)
endif()

if (NOT PLATFORM STREQUAL "host" AND SDK_STANDALONE)
    add_library(gpio_compile_check OBJECT
        ${CMAKE_CURRENT_LIST_DIR}/tests/compile_check.cpp
    )

    target_link_libraries(gpio_compile_check PRIVATE
        gpio_driver
    )
endif()
