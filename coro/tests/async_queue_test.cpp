#include <gtest/gtest.h>

#include <coro/async_queue.hpp>
#include <coro/coro_task.hpp>

#include <memory>

using namespace driver::async;

struct non_default_message {
    explicit non_default_message(int value) : value(value) {}

    non_default_message() = delete;
    non_default_message(const non_default_message&) = delete;
    non_default_message& operator=(const non_default_message&) = delete;
    non_default_message(non_default_message&&) noexcept = default;
    non_default_message& operator=(non_default_message&&) noexcept = default;

    int value;
};

TEST(AsyncQueue, PreservesFifoOrderAndReportsFull) {
    async_queue<int, 3> queue;

    EXPECT_EQ(queue.capacity(), 3U);
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());

    EXPECT_TRUE(queue.try_push(1));
    EXPECT_TRUE(queue.try_push(2));
    EXPECT_TRUE(queue.try_push(3));
    EXPECT_TRUE(queue.full());
    EXPECT_FALSE(queue.try_push(4));

    EXPECT_EQ(queue.try_pop(), 1);
    EXPECT_EQ(queue.try_pop(), 2);
    EXPECT_EQ(queue.try_pop(), 3);
    EXPECT_FALSE(queue.try_pop().has_value());
    EXPECT_TRUE(queue.empty());
}

TEST(AsyncQueue, CoroutineWaitsUntilProcess) {
    async_queue<int, 2> queue;
    std::optional<int> received;

    launch_task([](auto& queue, auto& received) -> coro_task {
        received = co_await queue.async_pop();
    }, queue, received).resume();

    EXPECT_FALSE(received.has_value());
    EXPECT_TRUE(queue.try_push(42));
    EXPECT_FALSE(received.has_value());

    queue.process();

    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(*received, 42);
    EXPECT_TRUE(queue.empty());
}

TEST(AsyncQueue, CoroutineDoesNotSuspendWhenItemAlreadyAvailable) {
    async_queue<int, 2> queue;
    ASSERT_TRUE(queue.try_push(42));
    std::optional<int> received;

    launch_task([](auto& queue, auto& received) -> coro_task {
        received = co_await queue.async_pop();
    }, queue, received).resume();

    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(*received, 42);
}

TEST(AsyncQueue, ProcessDoesNothingWithoutAnItem) {
    async_queue<int, 2> queue;
    bool resumed = false;

    launch_task([](auto& queue, auto& resumed) -> coro_task {
        (void)co_await queue.async_pop();
        resumed = true;
    }, queue, resumed).resume();

    queue.process();

    EXPECT_FALSE(resumed);
}

TEST(AsyncQueue, CoroutineCanWaitForSeveralItems) {
    async_queue<int, 2> queue;
    int total = 0;

    launch_task([](auto& queue, auto& total) -> coro_task {
        total += co_await queue.async_pop();
        total += co_await queue.async_pop();
    }, queue, total).resume();

    ASSERT_TRUE(queue.try_push(20));
    queue.process();
    EXPECT_EQ(total, 20);

    ASSERT_TRUE(queue.try_push(22));
    queue.process();
    EXPECT_EQ(total, 42);
}

TEST(AsyncQueue, SupportsMoveOnlyMessages) {
    async_queue<std::unique_ptr<int>, 1> queue;
    std::unique_ptr<int> received;

    launch_task([](auto& queue, auto& received) -> coro_task {
        received = co_await queue.async_pop();
    }, queue, received).resume();

    ASSERT_TRUE(queue.try_push(std::make_unique<int>(42)));
    queue.process();

    ASSERT_NE(received, nullptr);
    EXPECT_EQ(*received, 42);
}

TEST(AsyncQueue, SupportsMessagesWithoutDefaultConstructor) {
    async_queue<non_default_message, 1> queue;

    ASSERT_TRUE(queue.try_emplace(42));
    const auto message = queue.try_pop();

    ASSERT_TRUE(message.has_value());
    EXPECT_EQ(message->value, 42);
}

TEST(AsyncQueue, OnlyOneWaiterAllowed) {
    async_queue<int, 2> queue;
    bool first = false;
    bool second = false;

    launch_task([](auto& queue, auto& resumed) -> coro_task {
        (void)co_await queue.async_pop();
        resumed = true;
    }, queue, first).resume();

#ifndef NDEBUG
    EXPECT_DEATH({
        launch_task([](auto& queue, auto& resumed) -> coro_task {
            (void)co_await queue.async_pop();
            resumed = true;
        }, queue, second).resume();
    }, "Only one waiter");
#endif
}
