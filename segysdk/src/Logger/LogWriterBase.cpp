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

#include "../Logger/LogWriterBase.h"

#include <iostream>

namespace segysdk::logger
{

LogWriterBase::LogWriterBase()
  : worker_(1)
{
}

void
LogWriterBase::run()
{
    if (!workerRunning_.load()) {
        // Spawn a thread consuming messages
        workerRunning_ = true;
        worker_.addWorker(std::thread(&LogWriterBase::writeMessagesThread, this));
    }
}

LogWriterBase::~LogWriterBase()
{
    stopWriter();
}

void
LogWriterBase::stopWriter()
{
    if (workerRunning_.load()) {
        workerRunning_ = false;
        worker_.joinAll();
    }
}

void
LogWriterBase::addMessages(const PoolMessages& messages)
{
    std::unique_lock lock(mtx_);
    if (messages.size() + messages_.size() > maxMessages_) {
        size_t toErase = messages.size() + messages_.size() - maxMessages_;
        messages_.erase(messages_.begin(), std::next(messages_.begin(), toErase));
    }
    messages_.insert(messages_.end(), messages.begin(), messages.end());

    messagesPresent_ = true;
    cvMessagesReady_.notify_one();
}

void
LogWriterBase::writeMessagesThread()
{
    while (workerRunning_.load()) {
        PoolMessages copiedMessages = {};
        {
            std::unique_lock lock(mtx_);
            if (cvMessagesReady_.wait_for(lock, waitForMessages_,
                                          [this]() { return messagesPresent_ == true; })) {
                // copy messages to unblock write method in case of new messages
                auto toRead = std::min(messages_.size(), messagesChunk_);
                auto end = std::next(messages_.begin(), toRead);
                copiedMessages.assign(messages_.begin(), end);
                messages_.erase(messages_.begin(), end);
                if (messages_.empty()) {
                    messagesPresent_ = false;
                }
            }
        } // end of lock

        if (!copiedMessages.empty()) {
            write(copiedMessages);
        }
        std::this_thread::yield();
    }
}

void
LogWriterBase::store(const PoolMessages& messages)
{
    if (!messages.empty()) {
        addMessages(messages);
    }
}

} // namespace segysdk::logger
