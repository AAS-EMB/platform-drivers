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

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        fired = true;
    };

    auto t = coro();

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

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        fired = true;
    };

    auto t = coro();

    ct.process();
    EXPECT_FALSE(fired);

    steady_clock::traits::advance(100);
    ct.process();
    EXPECT_TRUE(fired);
}

TEST_F(CoroTimer, MultipleTimersWork) {
    coro_timer ct1(50ns);
    coro_timer ct2(100ns);
    steady_clock::traits::reset();
    bool a = false;
    bool b = false;

    auto coro1 = [&]() -> coro_task {
        co_await ct1.async_wait();
        a = true;
    };

    auto coro2 = [&]() -> coro_task {
        co_await ct2.async_wait();
        b = true;
    };

    auto t1 = coro1();
    auto t2 = coro2();

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

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        ++count;
    };

    auto t = coro();

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

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        fired = true;
    };

    auto t = coro();

    EXPECT_TRUE(fired);
}

TEST_F(CoroTimer, ResetDeadline) {
    coro_timer ct(100ns);
    bool fired = false;

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        fired = true;
    };

    auto t = coro();

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

    auto coro = [&]() -> coro_task {
        co_await ct.async_wait();
        fired = true;
    };

    auto t = coro();

    steady_clock::traits::advance(200);
    EXPECT_FALSE(fired);
}

TEST_F(CoroTimer, CanBeReused) {
    coro_timer ct(100ns);
    int count = 0;

    auto coro1 = [&]() -> coro_task {
        ct.expires_after(100ns);
        co_await ct.async_wait();
        ++count;
    };

    auto t1 = coro1();

    driver::steady_clock::traits::advance(100);
    ct.process();
    EXPECT_EQ(count, 1);

    auto coro2 = [&]() -> coro_task {
        ct.expires_after(200ns);
        co_await ct.async_wait();
        ++count;
    }();

    driver::steady_clock::traits::advance(200);
    ct.process();
    EXPECT_EQ(count, 2);
}

// TEST_F(TimerTest, OnlyOneWaiterAllowed) {
//     using namespace driver::async;

//     steady_timer t(100ns);
//     bool a = false, b = false;

//     coro_task t1 = [&]() -> coro_task {
//         co_await t.async_wait();
//         a = true;
//     }();

// #ifndef NDEBUG
//     EXPECT_DEATH({
//         coro_task t2 = [&]() -> coro_task {
//             co_await t.async_wait();
//             b = true;
//         }();
//     }, ".*");
// #endif
// }

// TEST_F(TimerTest, TimerOutlivesCoroutine) {
//     using namespace driver::async;

//     steady_timer t(100ns);

//     {
//         coro_task task = [&]() -> coro_task {
//             co_await t.async_wait();
//         }();
//     }

//     EXPECT_NO_THROW({
//         driver::steady_clock::traits::advance(200);
//         t.poll();
//     });
// }

// TEST_F(TimerTest, CancelPreventsResume) {
//     using namespace driver::async;

//     bool fired = false;
//     steady_timer t(100ns);

//     coro_task task = [&]() -> coro_task {
//         co_await t.async_wait();
//         fired = true;
//     }();

//     t.cancel();

//     driver::steady_clock::traits::advance(200);
//     t.poll();

//     EXPECT_FALSE(fired);
// }