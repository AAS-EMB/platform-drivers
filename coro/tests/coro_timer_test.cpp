#include <gtest/gtest.h>
#include <coro/coro_timer.hpp>
#include <coro/coro_task.hpp>

using namespace driver;
using namespace driver::async;
using namespace std::chrono_literals;

struct CoroTimer : public ::testing::Test {
    void SetUp() override {
        driver::steady_clock::traits::reset();
    }
};

TEST_F(CoroTimer, DoesNotFireTooEarly) {
    coro_timer ct(100ns);
    bool fired = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    ct.process();
    EXPECT_FALSE(fired);

    steady_clock::traits::advance(50);
    ct.process();
    EXPECT_FALSE(fired);

    steady_clock::traits::advance(49);
    ct.process();
    EXPECT_FALSE(fired);
}

TEST_F(CoroTimer, FiresAtDeadline) {
    coro_timer ct(100ns);
    bool fired = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    ct.process();
    EXPECT_FALSE(fired);

    steady_clock::traits::advance(100);
    ct.process();
    EXPECT_TRUE(fired);
}

TEST_F(CoroTimer, MultipleTimersWork) {
    coro_timer ct1(50ns);
    coro_timer ct2(100ns);
    bool a = false;
    bool b = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct1, a).resume();

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct2, b).resume();

    steady_clock::traits::advance(60);
    ct1.process();
    ct2.process();
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);

    steady_clock::traits::advance(40);
    ct1.process();
    ct2.process();
    EXPECT_TRUE(b);
}

TEST_F(CoroTimer, FiresOnlyOnce) {
    coro_timer ct(100ns);
    int count = 0;

    launch_task([](auto & ct, auto & count) -> coro_task {
        co_await ct.async_wait();
        ++count;
    }, ct, count).resume();

    driver::steady_clock::traits::advance(100);
    ct.process();
    EXPECT_EQ(count, 1);

    ct.process();
    EXPECT_EQ(count, 1);
}

TEST_F(CoroTimer, AwaitReadyIfAlreadyExpired) {
    coro_timer ct(100ns);
    bool fired = false;

    driver::steady_clock::traits::advance(200);

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    EXPECT_TRUE(fired);
}

TEST_F(CoroTimer, ResetDeadline) {
    coro_timer ct(100ns);
    bool fired = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    ct.expires_after(200ns);

    driver::steady_clock::traits::advance(100);
    ct.process();
    EXPECT_FALSE(fired);

    driver::steady_clock::traits::advance(100);
    ct.process();
    EXPECT_TRUE(fired);
}

TEST_F(CoroTimer, NoPollNoFire) {
    coro_timer ct(100ns);
    bool fired = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    steady_clock::traits::advance(200);
    EXPECT_FALSE(fired);
}

TEST_F(CoroTimer, CanBeReused) {
    coro_timer ct(100ns);
    int count = 0;

    launch_task([](auto & ct, auto & count) -> coro_task {
        co_await ct.async_wait();
        ++count;
    }, ct, count).resume();

    driver::steady_clock::traits::advance(100);
    ct.process();
    EXPECT_EQ(count, 1);

    ct.expires_after(200ns);

    launch_task([](auto & ct, auto & count) -> coro_task {
        co_await ct.async_wait();
        ++count;
    }, ct, count).resume();

    driver::steady_clock::traits::advance(200);
    ct.process();
    EXPECT_EQ(count, 2);
}

TEST_F(CoroTimer, OnlyOneWaiterAllowed) {
    coro_timer ct(100ns);

    launch_task([](auto & ct) -> coro_task {
        co_await ct.async_wait();
    }, ct).resume();

#ifndef NDEBUG
    EXPECT_DEATH({
        launch_task([](auto & ct) -> coro_task {
            co_await ct.async_wait();
        }, ct).resume();
    }, "Only one waiter");
#endif
}

TEST_F(CoroTimer, TimerOutlivesCoroutine) {
    coro_timer ct(100ns);

    {
        [[maybe_unused]] auto task = launch_task([](auto & ct) -> coro_task {
            co_await ct.async_wait();
        }, ct);
    }

    EXPECT_NO_THROW({
        driver::steady_clock::traits::advance(200);
        ct.process();
    });
}

TEST_F(CoroTimer, CancelPreventsResume) {
    coro_timer ct(100ns);
    bool fired = false;

    launch_task([](auto & ct, auto & fired) -> coro_task {
        co_await ct.async_wait();
        fired = true;
    }, ct, fired).resume();

    ct.cancel();

    driver::steady_clock::traits::advance(200);
    ct.process();

    EXPECT_FALSE(fired);
}