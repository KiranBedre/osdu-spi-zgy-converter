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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>

namespace segysdk::concurrent
{

namespace details
{

struct SEGYSDK_DLL_PUBLIC SemaphoreCounter final
{
    SemaphoreCounter(uint64_t countStart, size_t decreaseTimeout);

    void Increase();
    bool DecreaseAndWaitWhenZero();

    static std::unique_ptr<SemaphoreCounter>
    Create(uint64_t countStart, size_t decreaseTimeout);

    SemaphoreCounter(SemaphoreCounter const& other) = delete;
    SemaphoreCounter& operator=(SemaphoreCounter const& other) = delete;
    SemaphoreCounter(SemaphoreCounter&& other) = delete;
    SemaphoreCounter& operator=(SemaphoreCounter&& other) = delete;

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    uint64_t _counter;
    std::chrono::milliseconds const _decreaseTimeout;
};

} // namespace details

template<typename BUFFER_ITEM>
struct BufferQueue
{
    using buffer_item_t = BUFFER_ITEM;
    using buffer_queue_t = std::list<buffer_item_t>;

    BufferQueue(uint64_t size, size_t decreaseTimeout);
    BufferQueue(uint64_t size);

    bool Produce(buffer_item_t& buffer);
    bool Consume(buffer_item_t& buffer);

private:
    BufferQueue(BufferQueue const& other) = delete;
    BufferQueue& operator=(BufferQueue const& other) = delete;
    BufferQueue(BufferQueue&& other) = delete;
    BufferQueue& operator=(BufferQueue&& other) = delete;

private:
    std::unique_ptr<details::SemaphoreCounter> _mutex;
    std::unique_ptr<details::SemaphoreCounter> _fillCounter;
    std::unique_ptr<details::SemaphoreCounter> _emptyCounter;
    buffer_queue_t _queue;
    const static size_t DefaultTimeout = 3600000;
};

} // namespace segysdk::concurrent

namespace segysdk::concurrent
{

//-----------------------------------------------------------------------------
// BufferQueue class implementation
//-----------------------------------------------------------------------------

template<typename BUFFER_ITEM>
inline BufferQueue<BUFFER_ITEM>::BufferQueue(uint64_t size)
  : BufferQueue(size, DefaultTimeout)
{
}

template<typename BUFFER_ITEM>
inline BufferQueue<BUFFER_ITEM>::BufferQueue(uint64_t size, size_t decreaseTimeout)
{
    _fillCounter = details::SemaphoreCounter::Create(0, decreaseTimeout);
    _emptyCounter = details::SemaphoreCounter::Create(size, decreaseTimeout);
    _mutex = details::SemaphoreCounter::Create(1, decreaseTimeout);
}

template<typename BUFFER_ITEM>
inline bool
BufferQueue<BUFFER_ITEM>::Produce(buffer_item_t& buffer)
{
    if (not _emptyCounter->DecreaseAndWaitWhenZero()) {
        return false;
    }

    if (not _mutex->DecreaseAndWaitWhenZero()) {
            _emptyCounter->Increase();
        return false;
    }

    _queue.push_back(buffer);
    _mutex->Increase();
    _fillCounter->Increase();

    return true;
}

template<typename BUFFER_ITEM>
inline bool
BufferQueue<BUFFER_ITEM>::Consume(buffer_item_t& buffer)
{
    if (not _fillCounter->DecreaseAndWaitWhenZero()) {
        return false;
    }

    if (not _mutex->DecreaseAndWaitWhenZero()) {
        _fillCounter->Increase();
        return false;
    }
    std::swap(_queue.front(), buffer);

    _queue.pop_front();
    _mutex->Increase();
    _emptyCounter->Increase();

    return true;
}

} // namespace segysdk::concurrent
