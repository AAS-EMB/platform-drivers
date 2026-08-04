#include <coro/coro_timer.hpp>
#include <coro/coro_task.hpp>
#include <coro/async_flag.hpp>
#include <coro/async_queue.hpp>

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

    async_queue<int, 4> queue;
    [[maybe_unused]] const auto pushed = queue.try_push(1);
    launch_task([](auto& queue) -> coro_task {
        (void)co_await queue.async_pop();
    }, queue).resume();
    queue.process();
}
