/*
Copyright 2024 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "Semaphore/Semaphore.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <future>
#include <thread>
#include <vector>

namespace segysdk::tests
{

constexpr auto numStrides = 20;
constexpr auto maxTasks = std::ptrdiff_t{4};
constexpr auto taskDelayMs = std::int64_t{100};

TEST(SemaphoreTest, MaxTasks)
{
    auto strides = std::vector<std::future<void>>{};
    auto limiter = Semaphore{maxTasks};
    const auto start = std::chrono::steady_clock::now();
    for (auto i = 0; i < numStrides; ++i) {
        strides.emplace_back(std::async(std::launch::async, [&limiter] {
            const auto cs = CriticalSection{limiter};
            std::this_thread::sleep_for(std::chrono::milliseconds{taskDelayMs});
        }));
    }
    for (auto& stride : strides) {
        stride.get();
    }
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    EXPECT_GE(elapsedMs, numStrides * taskDelayMs / maxTasks);
}

} // namespace segysdk::tests
