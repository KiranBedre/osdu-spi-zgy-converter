/*
Copyright 2022 Schlumberger

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

#include "../Concurrent/ThreadPool.h"

#include <functional>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

namespace segysdk::concurrent::tests
{

using segysdk::concurrent::ThreadPool;

namespace details
{

static constexpr size_t const numberOfThreads = 7U;

} // namespace details

struct ThreadPoolUnitTests : public testing::Test
{
    template<size_t THREAD_COUNT>
    void TestFunctionPool()
    {
        EXPECT_GE(THREAD_COUNT, 3U);

        // The thread pool under the tests
        ThreadPool tp(THREAD_COUNT);

        // The registry to check if every cell has been assigned with a thread
        std::mutex registryMutex;
        std::vector<size_t> registry;

        // Creating all the threads except the first and the last one
        for (size_t idx = 0; idx < THREAD_COUNT; ++idx) {

            if (0 == idx) {
                continue; // skip the first cell
            }

            if ((THREAD_COUNT - 1) == idx) {
                continue; // skip the last cell
            }

            auto const added = tp.setWorker(idx, std::thread{ThreadPoolUnitTests::addToRegistry, std::ref(registryMutex), std::ref(registry), idx});
            EXPECT_EQ(added, true);
        }

        // Creating the first thread
        {
            auto const addedAsFirst = tp.addWorker(std::thread{ThreadPoolUnitTests::addToRegistry, std::ref(registryMutex), std::ref(registry), 0U});
            EXPECT_EQ(addedAsFirst, true);
        }

        // Creating the last thread
        {
            auto const addedAsLast = tp.addWorker(std::thread{ThreadPoolUnitTests::addToRegistry, std::ref(registryMutex), std::ref(registry), (THREAD_COUNT - 1)});
            EXPECT_EQ(addedAsLast, true);
        }

        // Joining all the threads explicitly
        tp.joinAll();

        // Check if every cell has been assigned with a thread
        EXPECT_EQ(registry.size(), THREAD_COUNT);
        for (size_t idx = 0; idx < THREAD_COUNT; ++idx) {
            EXPECT_NE(registry.cend(), std::find(registry.cbegin(), registry.cend(), idx));
        }
    }

    template<size_t THREAD_COUNT>
    void TestLambdaPool()
    {
        EXPECT_GE(THREAD_COUNT, 3U);

        // The thread pool under the tests
        ThreadPool tp(THREAD_COUNT);

        // The registry to check if every cell has been assigned with a thread
        std::mutex registryMutex;
        std::vector<size_t> registry;

        // Creating all the threads except the first and the last one
        for (size_t idx = 0; idx < THREAD_COUNT; ++idx) {

            if (0 == idx) {
                continue; // skip the first cell
            }

            if ((THREAD_COUNT - 1) == idx) {
                continue; // skip the last cell
            }

            auto const added = tp.setWorker(idx, std::thread{[idx, &registryMutex, &registry]() {
                                                ThreadPoolUnitTests::addToRegistry(registryMutex, registry, idx);
                                            }});
            EXPECT_EQ(added, true);
        }

        // Creating the first thread
        {
            auto const addedAsFirst = tp.addWorker(std::thread{[&registryMutex, &registry]() {
                ThreadPoolUnitTests::addToRegistry(registryMutex, registry, 0);
            }});
            EXPECT_EQ(addedAsFirst, true);
        }

        // Creating the last thread
        {
            auto const addedAsLast = tp.addWorker(std::thread{[&registryMutex, &registry]() {
                ThreadPoolUnitTests::addToRegistry(registryMutex, registry, (THREAD_COUNT - 1));
            }});
            EXPECT_EQ(addedAsLast, true);
        }

        // Joining all the threads explicitly
        tp.joinAll();

        // Check if every cell has been assigned with a thread
        EXPECT_EQ(registry.size(), THREAD_COUNT);
        for (size_t idx = 0; idx < THREAD_COUNT; ++idx) {
            EXPECT_NE(registry.cend(), std::find(registry.cbegin(), registry.cend(), idx));
        }
    }

private:
    static void addToRegistry(std::mutex& registryMutex, std::vector<size_t>& registry, size_t value)
    {
        std::unique_lock<std::mutex> lock(registryMutex);
        registry.push_back(value);
    }
};

TEST_F(ThreadPoolUnitTests, testPoolForFunction)
{
    TestFunctionPool<details::numberOfThreads>();
}

TEST_F(ThreadPoolUnitTests, testPoolForLambda)
{
    TestLambdaPool<details::numberOfThreads>();
}

} // namespace segysdk::concurrent::tests
