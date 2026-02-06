#include <gtest/gtest.h>
#include <coro/async_flag.hpp>
#include <coro/coro_task.hpp>

using namespace driver::async;

TEST(AsyncFlag, CoroutineWaitsUntilSet) {
    async_flag flag;
    std::atomic_bool resumed = false;

    launch_task([](auto & flag, auto & resumed) -> coro_task {
        co_await flag;
        resumed = true;
    }, flag, resumed).resume();

    EXPECT_FALSE(resumed);

    flag = true;

    EXPECT_TRUE(resumed);
}

TEST(AsyncFlag, ImmediateResumeIfAlreadySet) {
    async_flag flag;

    flag = true;

    bool resumed = false;

    launch_task([](auto & flag, auto & resumed) -> coro_task {
        co_await flag;
        resumed = true;
    }, flag, resumed).resume();

    EXPECT_TRUE(resumed);
}

TEST(AsyncFlag, ClearResetsFlag) {
    async_flag flag;

    flag = true;
    flag = false;

    EXPECT_FALSE(flag);

    bool resumed = false;

    launch_task([](auto & flag, auto & resumed) -> coro_task {
        co_await flag;
        resumed = true;
    }, flag, resumed).resume();

    EXPECT_FALSE(resumed);
}

TEST(AsyncFlag, MultipleSetIsSafe) {
    async_flag flag;
    bool resumed = false;

    launch_task([](auto & flag, auto & resumed) -> coro_task {
        co_await flag;
        resumed = true;
    }, flag, resumed).resume();

    flag = true;
    flag = true;
    flag = true;

    EXPECT_TRUE(resumed);
}

TEST(AsyncFlag, DoubleSetDoesNotResumeTwice) {
    async_flag flag;
    int counter = 0;

    launch_task([](auto & flag, auto & counter) -> coro_task {
        co_await flag;
        ++counter;
    }, flag, counter).resume();

    flag = true;
    EXPECT_EQ(counter, 1);

    flag = true;
    EXPECT_EQ(counter, 1);
}

TEST(AsyncFlag, ResetAllowsReuse) {
    async_flag flag;
    int counter = 0;

    launch_task([](auto & flag, auto & counter) -> coro_task {
        co_await flag;
        ++counter;
    }, flag, counter).resume();

    flag = true;
    EXPECT_EQ(counter, 1);

    flag = false;

    launch_task([](auto & flag, auto & counter) -> coro_task {
        co_await flag;
        ++counter;
    }, flag, counter).resume();

    EXPECT_EQ(counter, 1);

    flag = true;
    EXPECT_EQ(counter, 2);
}

TEST(AsyncFlag, OnlyOneWaiterAllowed) {
    async_flag flag;
    bool a = false;
    bool b = false;

    launch_task([](auto & flag, auto & f) -> coro_task {
        co_await flag;
        f = true;
    }, flag, a).resume();

#ifndef NDEBUG
    EXPECT_DEATH({
        launch_task([](auto & flag, auto & f) -> coro_task {
            co_await flag;
            f = true;
        }, flag, b).resume();
    }, "Only one waiter");
#endif
}

TEST(AsyncFlag, CoroutineOutlivesFlag) {
    launch_task([]() -> coro_task {
        async_flag flag;
        co_await flag;
    }).resume();

    SUCCEED();
}