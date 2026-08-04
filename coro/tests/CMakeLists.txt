
add_executable(coro_lib_tests
    async_flag_test.cpp
    async_queue_test.cpp
    coro_timer_test.cpp
)

target_link_libraries(coro_lib_tests
    PRIVATE
        coro_lib
        gtest
        gmock
        gtest_main
        chrono_clocks
)

add_test(
    NAME coro_lib_tests
    COMMAND coro_lib_tests
)
