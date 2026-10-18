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

#pragma once

#include "../Logger/Message.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace segysdk::logger
{

class SEGYSDK_DLL_PUBLIC MessagePool
{
public:
    using MessagePoolDataStorage = std::deque<std::unique_ptr<Message>>;

    MessagePool();
    ~MessagePool();

    MessagePool(const MessagePool&) = delete;
    MessagePool& operator=(const MessagePool&) = delete;

    MessagePool& operator=(MessagePool&&) = delete;
    MessagePool(MessagePool&&) = delete;

    bool add(std::unique_ptr<Message> msg);
    MessagePoolDataStorage consume_front(size_t elements);
    bool clear();
    size_t size() const;

private:
    static constexpr auto timeoutDuration_ = std::chrono::milliseconds(100);
    static constexpr size_t max_pool_messages_ = 2048;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool readyToConsume_ = false;
    MessagePoolDataStorage messages_;
};

} // namespace segysdk::logger
