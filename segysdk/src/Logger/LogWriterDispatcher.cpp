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

#include "../Logger/LogWriterDispatcher.h"

#include <algorithm>
#include <iostream>
#include <thread>

namespace segysdk::logger
{

LogWriterDispatcher::LogWriterDispatcher(std::weak_ptr<MessagePool> pool)
  : consumersCounter_(0, consumersWaitDuration_.count()),
    pool_(pool),
    workerThreads_(workerThreadsNumb_)
{
    for (std::size_t i = 0; i < dispatcherThreadsNumb_; ++i) {
        readyToConsume_[i] = false;
    }
}

LogWriterDispatcher::~LogWriterDispatcher()
{
    stopDispatcher();
}

void
LogWriterDispatcher::addLogWriter(std::shared_ptr<LogWriterBase> writer)
{
    if (isRunning_.load()) {
        return;
    }
    writers_.emplace_back(writer);
}

void
LogWriterDispatcher::runDispatcher()
{
    if (isRunning_.load()) {
        return;
    }

    isRunning_ = true;
    // Spawn workers waiting for messages to write
    for (auto& worker : writers_) {
        worker->run();
    }
    // Thread to consume Log Messages from the Pool
    auto consumeMessages = [this]() {
        while (isRunning_.load()) {
            auto pool = pool_.lock();
            if (pool) {
                auto messages = pool->consume_front(LogWriterSettings::readSize_);
                if (!messages.empty()) {
                    assignMessagesFromPool(messages);

                    // Wait until all messages are consumed by dispatcher writer threads
                    for (size_t waits = 0; isRunning_.load() && waits < dispatcherThreadsNumb_; ++waits) {
                        while (!consumersCounter_.DecreaseAndWaitWhenZero()) {
                            // Not yet consumed - check whether we're not requested to exit
                            if (!isRunning_.load()) {
                                break;
                            }
                        }
                    }
                }
            }
            std::this_thread::yield();
        }
    };
    workerThreads_.addWorker(std::thread(consumeMessages));

    auto dispatch = [this](const LogWriterReferenceList& writerReferences, size_t dispatchIdx) {
        while (isRunning_.load()) {
            if (writeMessagesFor(writerReferences, dispatchIdx)) {
                consumersCounter_.Increase();
            }
            std::this_thread::yield();
        }
    };
    size_t toDistribute = writers_.size() / dispatcherThreadsNumb_;
    size_t writersDistributed = 0;
    for (size_t dispatchIdx = 0; dispatchIdx < dispatcherThreadsNumb_; ++dispatchIdx) {
        if (dispatchIdx == dispatcherThreadsNumb_ - 1) {
            // The last one receives all left
            toDistribute = toDistribute + writers_.size() % dispatcherThreadsNumb_;
        }
        LogWriterReferenceList writerReferences(std::next(writers_.begin(), writersDistributed),
                                                std::next(writers_.begin(), writersDistributed + toDistribute));
        workerThreads_.addWorker(std::thread(dispatch, writerReferences, dispatchIdx));
        writersDistributed += toDistribute;
    }
}

void
LogWriterDispatcher::stopDispatcher()
{
    if (isRunning_.load()) {
        isRunning_ = false;
        workerThreads_.joinAll();
        clearWriters();
        messages_.clear();
    }
}

void
LogWriterDispatcher::clearWriters()
{
    writers_.clear();
}

bool
LogWriterDispatcher::writeMessagesFor(const LogWriterReferenceList& writerReferences, size_t dispatchIdx)
{
    std::shared_lock lock(messagesMutex_);
    if (cvMessagesReady_.wait_for(lock, timeoutDuration_,
                                  [this, &dispatchIdx]() { return readyToConsume_[dispatchIdx]; })) {
        for (const auto& writer : writerReferences) {
            writer.get()->store(messages_);
        }
        readyToConsume_[dispatchIdx] = false;
        return true;
    }
    return false;
}

void
LogWriterDispatcher::assignMessagesFromPool(segysdk::logger::MessagePool::MessagePoolDataStorage& messages)
{
    std::lock_guard lock(messagesMutex_);
    // Transfer ownership of the messages
    messages_.assign(std::make_move_iterator(messages.begin()),
                     std::make_move_iterator(messages.end()));
    std::for_each(readyToConsume_.begin(), readyToConsume_.end(), [](auto& ready) { ready.second = true; });
    cvMessagesReady_.notify_all();
}

} // namespace segysdk::logger
