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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../Concurrent/BufferQueue.h"
#include "../Concurrent/ThreadPool.h"
#include "../Logger/LogWriterBase.h"
#include "../Logger/MessagePool.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace segysdk::logger
{

/**
 * @brief Class responsible for spawning log writer threads and storing logged messages. Is created and used by the Logger.
 *
 **/
class SEGYSDK_DLL_PUBLIC LogWriterDispatcher
{
public:
    using spLogWriterBase = std::shared_ptr<LogWriterBase>;
    using LogWriterList = std::list<spLogWriterBase>;
    using LogWriterReferenceList = std::list<std::reference_wrapper<spLogWriterBase>>;

    /**
     * @brief Creates an object.
     *
     * @param pool MessagePool that is used to store the logged messages.
     *
     **/
    explicit LogWriterDispatcher(std::weak_ptr<MessagePool> pool);

    LogWriterDispatcher(const LogWriterDispatcher&) = delete;
    LogWriterDispatcher operator=(const LogWriterDispatcher&) = delete;

    ~LogWriterDispatcher();

    /**
     * @brief Adds LogWriterBase objects to the Dispatcher. Should happen before runDispatcher is called.
     *
     * @param writer object to be added.
     *
     **/
    void addLogWriter(std::shared_ptr<LogWriterBase> writer);

    /**
     * @brief Runs the Dispatcher. Spawns log writer threads and a thread consuming messages from the Logger.
     *        Log writer threads are calling write of LogWriterBase objects that were added previously.
     *
     **/
    void runDispatcher();

    /**
     * @brief Stops the Dispatcher. Waits for all log writer threads to finish the job.
     *
     **/
    void stopDispatcher();

private:
    void clearWriters();

    bool writeMessagesFor(const LogWriterReferenceList&, size_t);
    void assignMessagesFromPool(segysdk::logger::MessagePool::MessagePoolDataStorage& messages);

    struct LogWriterSettings
    {
        static constexpr size_t readSize_ = 32;
    };

    // timeout must be long enough to avoid release the lock in a valid waiting (not frozen). 
    // Small timeout could lead to race condition
    static constexpr auto timeoutDuration_ = std::chrono::milliseconds(30000);
    static constexpr auto consumersWaitDuration_ = std::chrono::milliseconds(30000);
    mutable std::shared_timed_mutex messagesMutex_;
    std::condition_variable_any cvMessagesReady_;
    std::map<size_t, bool> readyToConsume_;
    segysdk::concurrent::details::SemaphoreCounter consumersCounter_;
    LogWriterBase::PoolMessages messages_;

    std::weak_ptr<MessagePool> pool_;
    LogWriterList writers_ = {};

    std::atomic_bool isRunning_ = false;
    static constexpr size_t dispatcherThreadsNumb_ = 2;
    static constexpr size_t workerThreadsNumb_ = dispatcherThreadsNumb_ + 1;
    segysdk::concurrent::ThreadPool workerThreads_;
};

} // namespace segysdk::logger
