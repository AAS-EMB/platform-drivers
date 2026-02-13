#include <coro/coro_timer.hpp>
#include <coro/coro_task.hpp>
#include <coro/async_flag.hpp>

using namespace driver;
using namespace driver::async;
using namespace std::chrono_literals;

void compile_check() {
    coro_timer ct(100ns);

    launch_task([](auto & ct) -> coro_task {
        co_await ct.async_wait();
    }, ct).resume();

    launch_task([]() -> coro_task {
        async_flag flag;
        co_await flag;
    }).resume();
}