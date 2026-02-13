#pragma once
#include <coroutine>
#include <chrono_clocks/steady_clock.hpp>
#include <cassert>

namespace driver::async {

struct coro_timer {
    coro_timer() noexcept = default;
    coro_timer(const coro_timer&) noexcept = delete;
    coro_timer& operator=(const coro_timer&) noexcept = delete;
    explicit coro_timer(std::chrono::nanoseconds d) noexcept {
        expires_after(d);
    }

    void expires_after(std::chrono::nanoseconds d) noexcept {
        deadline_ = driver::steady_clock::now() + d;
    }

    void expires_at(driver::steady_clock::time_point tp) noexcept {
        deadline_ = tp;
    }

    void cancel() noexcept {
        handle_ = {};
        active_ = false;
    }

    struct awaiter {
        coro_timer & timer;

        bool await_ready() const noexcept {
            return driver::steady_clock::now() >= timer.deadline_;
        }

        bool await_suspend(std::coroutine_handle<> h) noexcept {
            assert(timer.handle_ == nullptr && "Only one waiter");
            timer.handle_ = h;
            timer.active_ = true;
            return true;
        }

        void await_resume() const noexcept {}
    };

    awaiter async_wait() noexcept {
        return awaiter{*this};
    }

    void process() noexcept {
        if (active_ and handle_ and driver::steady_clock::now() >= deadline_) {
            auto h  = handle_;
            handle_ = {};
            active_ = false;
            if (not h.done())
                h.resume();
        }
    }

private:
    driver::steady_clock::time_point deadline_{};
    std::coroutine_handle<> handle_{};
    bool active_{false};
};

}
