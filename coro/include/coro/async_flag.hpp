#pragma once
#include <coroutine>
#include <cstdbool>
#include <cassert>

namespace driver::async {

struct async_flag {
    async_flag() noexcept = default;
    async_flag(const async_flag&) noexcept = delete;
    async_flag& operator=(const async_flag&) noexcept = delete;
    ~async_flag() noexcept {
        waiter_ = {};
    }

    async_flag& operator=(bool value) noexcept {
        value_ = value;

        if (value_ and waiter_) {
            auto h  = waiter_;
            waiter_ = {};
            h.resume();
        }

        return *this;
    }

    operator bool() const {
        return value_;
    }

    bool await_ready() const noexcept {
        return value_;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        assert(waiter_ == nullptr && "Only one waiter");
        if (value_) {
            handle.resume();
        } else {
            waiter_ = handle;
        }
    }

    bool await_resume() noexcept {
        waiter_ = {};
        return value_;
    }

private:
    bool value_{false};
    std::coroutine_handle<> waiter_{};
};

}