#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <coroutine>
#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace driver::async {

// A bounded, allocation-free single-producer/single-consumer queue.
//
// The producer may call try_push() from an interrupt handler. The consumer
// calls process() from its normal execution context to resume a coroutine
// waiting in async_pop(); process() must not be called from an interrupt.
// Only one coroutine may wait for an item at a time.
template<typename T, std::size_t Capacity>
class async_queue {
    static_assert(Capacity > 0, "async_queue capacity must be greater than zero");
    static_assert(std::atomic_size_t::is_always_lock_free,
                  "async_queue requires lock-free size_t atomics");

    static constexpr std::size_t storage_size = Capacity + 1;
    using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

public:
    async_queue() noexcept = default;
    async_queue(const async_queue&) = delete;
    async_queue& operator=(const async_queue&) = delete;

    ~async_queue() noexcept {
        auto head = head_.load(std::memory_order_relaxed);
        const auto tail = tail_.load(std::memory_order_relaxed);
        while (head != tail) {
            std::destroy_at(slot(head));
            head = next(head);
        }
    }

    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }

    [[nodiscard]] bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

    [[nodiscard]] bool full() const noexcept {
        const auto tail = tail_.load(std::memory_order_relaxed);
        return next(tail) == head_.load(std::memory_order_acquire);
    }

    template<typename... Args>
    [[nodiscard]] bool try_emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        const auto tail = tail_.load(std::memory_order_relaxed);
        const auto next_tail = next(tail);

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        std::construct_at(slot_address(tail), std::forward<Args>(args)...);
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool try_push(const T& value)
        noexcept(std::is_nothrow_copy_constructible_v<T>) {
        return try_emplace(value);
    }

    [[nodiscard]] bool try_push(T&& value)
        noexcept(std::is_nothrow_move_constructible_v<T>) {
        return try_emplace(std::move(value));
    }

    [[nodiscard]] std::optional<T> try_pop()
        noexcept(std::is_nothrow_move_constructible_v<T>) {
        const auto head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt;
        }

        std::optional<T> value{std::move(*slot(head))};
        std::destroy_at(slot(head));
        head_.store(next(head), std::memory_order_release);
        return value;
    }

    class pop_awaiter {
    public:
        explicit pop_awaiter(async_queue& queue) noexcept : queue_(queue) {}

        [[nodiscard]] bool await_ready() const noexcept {
            return !queue_.empty();
        }

        bool await_suspend(std::coroutine_handle<> handle) noexcept {
            assert(queue_.waiter_ == nullptr && "Only one waiter");

            // A producer may have pushed between await_ready() and here.
            if (!queue_.empty()) {
                return false;
            }

            queue_.waiter_ = handle;

            // Avoid missing a producer that pushed just before the handle
            // was recorded. In that case continue without suspending.
            if (!queue_.empty()) {
                queue_.waiter_ = {};
                return false;
            }

            return true;
        }

        [[nodiscard]] T await_resume()
            noexcept(std::is_nothrow_move_constructible_v<T>) {
            auto value = queue_.try_pop();
            assert(value.has_value() && "async_queue resumed without an item");
            return std::move(*value);
        }

    private:
        async_queue& queue_;
    };

    [[nodiscard]] pop_awaiter async_pop() noexcept {
        return pop_awaiter{*this};
    }

    // Resume a waiting consumer after a producer has added an item. This must
    // be called from the consumer's normal execution context, never an ISR.
    void process() noexcept {
        if (empty() || !waiter_) {
            return;
        }

        const auto handle = std::exchange(waiter_, {});
        if (!handle.done()) {
            handle.resume();
        }
    }

private:
    [[nodiscard]] static constexpr std::size_t next(std::size_t index) noexcept {
        return (index + 1) % storage_size;
    }

    [[nodiscard]] T* slot(std::size_t index) noexcept {
        return std::launder(slot_address(index));
    }

    [[nodiscard]] T* slot_address(std::size_t index) noexcept {
        return reinterpret_cast<T*>(&storage_[index]);
    }

    std::array<storage_type, storage_size> storage_{};
    std::atomic_size_t head_{0};
    std::atomic_size_t tail_{0};
    std::coroutine_handle<> waiter_{};
};

} // namespace driver::async
