# STM32 Drivers & Platform Framework (Host + Embedded)

A modern, testable C++20 driver framework for STM32 microcontrollers with host-side simulation and unit testing.

This repository serves as a **long-term embedded platform project**, combining:
- low-level, deterministic drivers
- HAL-based peripherals where appropriate
- modern C++ abstractions
- and full host-side testability

---

## 🎯 Project Scope & Vision

The goal of this project is to build a **clean, scalable embedded driver framework** that can be:

- reused across multiple STM32 families
- tested on host without hardware
- selectively HAL-based or HAL-free
- suitable for production firmware

This is **not a HAL replacement** and **not a teaching example** —  
it is an exploration of **how modern C++ can be applied to real embedded systems**.

---

## 🧩 Supported Platforms

### Embedded Targets

| Board | MCU | Core |
|-----|----|----|
| STM32F1 Discovery | STM32F103 | Cortex-M3 |
| STM32F303 Discovery | STM32F303 | Cortex-M4F |
| STM32F767 Nucleo | STM32F767 | Cortex-M7 |

### Host Platform

- Linux / macOS / Windows
- GCC / Clang
- Full unit testing with GoogleTest / GoogleMock

---

## 🧠 Architectural Principles

### 1️⃣ Separation of Concerns

- **Public APIs** are platform-agnostic
- **Hardware-specific code** is isolated
- **Platform selection** is compile-time (CMake)
- **Business logic** never touches registers directly

---

### 2️⃣ HAL Usage Philosophy

HAL is used **selectively**, not dogmatically.

| Subsystem | Approach | Reason |
|--------|--------|--------|
| Time / Clocks | HAL-free (CMSIS) | Determinism, precision |
| SysTick / DWT | HAL-free | Cycle accuracy |
| CAN | HAL-based | Complexity, vendor support |
| UART / SPI | HAL or LL | Configurability |
| GPIO | Mixed | Simplicity vs control |

> **Rule of thumb**:  
> *If HAL adds value → use it.  
> If HAL hides important behavior → avoid it.*

---

## ⏱️ Time & Chrono Subsystem

The time subsystem is built around **`std::chrono` compatibility**.

### Features

- `steady_clock` abstraction
- Overflow-safe DWT cycle counter (STM32F7)
- SysTick-based clocks for F1 / F3
- Host implementation using `std::chrono::steady_clock`
- C++20 `Clock` concept for compile-time validation

### Why `std::chrono`?

- Familiar API
- Type safety
- Zero runtime overhead
- Easy mocking and testing

---

## 🧪 Testing Strategy

All non-hardware logic is tested **on the host**.

### What is tested

- Clock monotonicity
- Overflow handling
- Timer expiration logic
- Compile-time ClockConcept validation
- Algorithm-level correctness

### What is NOT tested on host

- Register correctness
- Peripheral electrical behavior
- DMA / IRQ timing

Those are validated on hardware.

---

## Build

The repository keeps vendor dependencies as submodules, but initializes only the
ones required by the selected platform. Do not use `--recurse-submodules` unless
you deliberately need every STM32 family.

```sh
# Host unit tests
./scripts/init-submodules.sh --platform host --with-tests
cmake --preset host-tests
cmake --build --preset host-tests
ctest --preset host-tests

# STM32F7 compile checks and HAL archive
./scripts/init-submodules.sh --platform stm32f7
cmake --preset stm32f7-release
cmake --build --preset stm32f7-release
```

Available presets are `host-tests`, `host-release`, and
`stm32f1-release` / `stm32f3-release` / `stm32f7-release`.

---

## Coroutine message queue

`driver::async::async_queue<T, Capacity>` is a bounded, allocation-free SPSC
queue for handing received messages from one producer to one coroutine
consumer. `try_push()` returns `false` when the queue is full; the caller can
then count or otherwise handle the dropped message.

An interrupt handler may push a message, but must not resume application code.
Call `process()` from the normal event loop to resume one coroutine waiting in
`co_await queue.async_pop()`.

```cpp
driver::async::async_queue<can_frame, 32> received_frames;

void CAN_RX_IRQHandler() {
    // Read the frame from hardware first.
    [[maybe_unused]] const bool queued = received_frames.try_push(read_frame());
}

driver::async::coro_task receive_loop() {
    for (;;) {
        auto frame = co_await received_frames.async_pop();
        handle(frame);
    }
}

void event_loop_iteration() {
    received_frames.process();
}
```

Only one coroutine may wait for each queue at a time. The queue uses lock-free
`size_t` atomics and is intended for one producer and one consumer.
