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

#include "../Concurrent/ThreadPool.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <list>
#include <memory>
#include <string>

namespace segysdk::logger
{

/**
 * @brief Abstract class for classes that would write logging messages.
 *
 **/
class SEGYSDK_DLL_PUBLIC LogWriterBase
{
public:
    using PoolMessages = std::list<std::shared_ptr<Message>>;
    LogWriterBase();
    virtual ~LogWriterBase();

    LogWriterBase(const LogWriterBase&) = delete;
    LogWriterBase& operator=(const LogWriterBase&) = delete;

    /**
     * @brief Stores incoming messages to internal queue.
     *
     * @details Incoming messages are stored to internal queue and the internal thread is unblocked for writing.
     *
     * @param messages Messages to be written.
     *
     **/
    virtual void store(const PoolMessages& messages);

    virtual void run();

protected:

    /**
     * @brief Writes messages from the internal queue
     *
     * @details All messages to be copied and stored to writer's message queue as result of the invocation.
     *          This method is called from the internal thread.
     *
     * @param messages Messages should be written.
     *
     **/
    virtual void write(const PoolMessages& messages) = 0;

    /**
     * @brief Stops internal writer thread
     *
     * @details During destruction all nested classes should call this method to stop base writer thread.
     *          This is because nested classes are destroyed earlier and there could be a case when base writer thread
     *          is still running write method which uses members/methods of nested class.
     *
     */
    virtual void stopWriter();

private:
    void addMessages(const PoolMessages& messages);

    void writeMessagesThread();

    // After reaching this limit old messages are gone
    static constexpr size_t maxMessages_ = 2048;
    // Size of the chunk of messages to be written in one call of slowWrite
    static constexpr size_t messagesChunk_ = 32;
    static constexpr std::chrono::milliseconds waitForMessages_{100};
    std::mutex mtx_;
    std::condition_variable cvMessagesReady_;
    bool messagesPresent_ = false;
    PoolMessages messages_;
    std::atomic_bool workerRunning_ = false;
    segysdk::concurrent::ThreadPool worker_;
};

} // namespace segysdk::logger
