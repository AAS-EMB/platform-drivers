add_library(coro_lib INTERFACE)

target_include_directories(coro_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(coro_lib INTERFACE
    cxx_std_20
)

# Platform-specific definitions
if (PLATFORM STREQUAL "host")
    target_link_libraries(coro_lib INTERFACE
        platform_flags
    )
elseif (PLATFORM STREQUAL "stm32f1")
    target_link_libraries(coro_lib INTERFACE
        stm32f1_hal
    )
elseif (PLATFORM STREQUAL "stm32f3")
    target_link_libraries(coro_lib INTERFACE
        stm32f3_hal
    )
elseif (PLATFORM STREQUAL "stm32f7")
    target_link_libraries(coro_lib INTERFACE
        stm32f7_hal
    )
endif()

if (BUILD_TESTS AND PLATFORM STREQUAL "host")
    add_subdirectory(tests)
endif()

# TODO
# if (NOT PLATFORM STREQUAL "host" AND SDK_STANDALONE)
#     add_library(chrono_compile_check OBJECT
#         ${CMAKE_CURRENT_LIST_DIR}/tests/compile_check.cpp
#     )

#     target_link_libraries(chrono_compile_check PRIVATE
#         coro_lib
#     )
# endif()
