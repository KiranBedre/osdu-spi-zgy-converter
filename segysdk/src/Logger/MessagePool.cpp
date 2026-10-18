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

#include "../Logger/MessagePool.h"

#include <algorithm>
#include <iostream>

namespace segysdk::logger
{

MessagePool::MessagePool()
{
}

MessagePool::~MessagePool()
{

}

bool
MessagePool::add(std::unique_ptr<Message> msg)
{
    std::lock_guard lock(mtx_);
    if (messages_.size() > max_pool_messages_) {
        return false;
    }
    messages_.push_back(std::move(msg));
    readyToConsume_ = true;
    cv_.notify_one();
    return true;
}

MessagePool::MessagePoolDataStorage
MessagePool::consume_front(size_t elements)
{
    std::unique_lock lock(mtx_);
    if (cv_.wait_for(lock, timeoutDuration_, [this]() { return readyToConsume_; })) {
        size_t elems_to_move = std::min(elements, messages_.size());
        MessagePoolDataStorage ret(std::make_move_iterator(messages_.begin()),
                                   std::make_move_iterator(messages_.begin() + elems_to_move));
        messages_.erase(messages_.begin(), messages_.begin() + elems_to_move);
        if (messages_.empty()) {
            readyToConsume_ = false;
        }
        return ret;
    }
    return {};
}

bool
MessagePool::clear()
{
    std::lock_guard lock(mtx_);
    readyToConsume_ = false;
    messages_.clear();
    return true;
}

size_t
MessagePool::size() const
{
    std::lock_guard lock(mtx_);
    return messages_.size();
}

} // namespace segysdk::logger
