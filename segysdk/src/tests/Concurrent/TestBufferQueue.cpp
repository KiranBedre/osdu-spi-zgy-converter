/*
Copyright 2021-2022 Schlumberger

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

#include "../Concurrent/BufferQueue.h"

#include <array>
#include <functional>
#include <thread>
#include <utility>

#include <gtest/gtest.h>

namespace segysdk::concurrent::tests
{

using segysdk::concurrent::BufferQueue;

namespace details
{

static constexpr size_t const lastItemID = std::numeric_limits<size_t>::max();
static constexpr size_t const sizeOfQueue = 16U;
static constexpr size_t const numberItemsToProduce = sizeOfQueue * 8U;
static constexpr size_t const numberOfConsumers = 4U;
static constexpr size_t const sleepDelay = 1U;
static constexpr size_t const numberOfIterationsMax = 10U;

using test_buffer_item_t = std::pair<size_t, size_t>;

template<typename BUFFER_ITEM>
struct is_last_queue_item_t;

template<>
struct is_last_queue_item_t<test_buffer_item_t>
{
    static bool value(test_buffer_item_t const& item)
    {
        return item.first == lastItemID;
    }
};

template<typename BUFFER_ITEM>
struct queue_item_factory_t;

template<>
struct queue_item_factory_t<test_buffer_item_t>
{
    using buffer_item_t = typename BufferQueue<test_buffer_item_t>::buffer_item_t;

    static buffer_item_t create(size_t index)
    {
        return buffer_item_t{index, 0U};
    }

    static buffer_item_t create_last_item()
    {
        return queue_item_factory_t::create(details::lastItemID);
    }
};

} // namespace details

struct BufferQueueUnitTests : public testing::Test
{
    using test_buffer_item_t = details::test_buffer_item_t;

    template<typename BUFFER_ITEM, template<typename> typename BUFFER_QUEUE>
    void TestItemsTransfer()
    {
        BUFFER_QUEUE<BUFFER_ITEM> bq(details::sizeOfQueue, details::sleepDelay);

        std::thread producer = std::thread(&BufferQueueUnitTests::DataProducer<BUFFER_ITEM, BUFFER_QUEUE>, this, std::ref(bq));

        std::array<std::thread, details::numberOfConsumers> consumer;
        for (size_t i = 0; i < consumer.size(); ++i) {
            consumer[i] = std::thread(&BufferQueueUnitTests::DataConsumer<BUFFER_ITEM, BUFFER_QUEUE>, this, std::ref(bq));
        }

        if (producer.joinable()) {
            producer.join();
        }

        for (size_t i = 0; i < consumer.size(); ++i) {
            if (consumer[i].joinable()) {
                consumer[i].join();
            }
        }
    }

protected:
    template<typename BUFFER_ITEM, template<typename> typename BUFFER_QUEUE>
    void DataProducer(BUFFER_QUEUE<BUFFER_ITEM>& bq)
    {
        for (size_t i = 0; i < details::numberItemsToProduce; ++i) {
            auto item = details::queue_item_factory_t<BUFFER_ITEM>::create(i);

            bool result{};
            size_t iterationsCount{};

            for (/*EMPTY*/;
                 not result and (iterationsCount < details::numberOfIterationsMax);
                 ++iterationsCount) {
                result = bq.Produce(item);
            }

            EXPECT_EQ(true, result);
            EXPECT_GE(details::numberOfIterationsMax, iterationsCount);
        }

        for (size_t j = 0; j < details::numberOfConsumers; ++j) {
            auto item = details::queue_item_factory_t<BUFFER_ITEM>::create_last_item();

            bool result{};
            size_t iterationsCount{};

            for (/*EMPTY*/;
                 not result and (iterationsCount < details::numberOfIterationsMax);
                 ++iterationsCount) {
                result = bq.Produce(item);
            }

            EXPECT_EQ(true, result);
            EXPECT_GE(details::numberOfIterationsMax, iterationsCount);
        }
    }
    template<typename BUFFER_ITEM, template<typename> typename BUFFER_QUEUE>
    void DataConsumer(BUFFER_QUEUE<BUFFER_ITEM>& bq)
    {
        using buffer_item_t = typename BUFFER_QUEUE<BUFFER_ITEM>::buffer_item_t;

        do {
            buffer_item_t item;

            bool result{};
            size_t iterationsCount{};

            for (/*EMPTY*/;
                 not result and (iterationsCount < details::numberOfIterationsMax);
                 ++iterationsCount) {
                result = bq.Consume(item);
            }

            EXPECT_EQ(true, result);
            EXPECT_GE(details::numberOfIterationsMax, iterationsCount);

            if (details::is_last_queue_item_t<BUFFER_ITEM>::value(item)) {
                break;
            }
        } while (true);
    }
};

TEST_F(BufferQueueUnitTests, testItemsTransferBufferQueue)
{
    TestItemsTransfer<test_buffer_item_t, BufferQueue>();
}

} // namespace segysdk::concurrent::tests
