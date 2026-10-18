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
#include "../Concurrent/ThreadPool.h"

#include <gtest/gtest.h>

#include <string>
#include <list>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace segysdk::utilities::tests
{

class MessagePoolTest : public ::testing::Test
{
protected:
    using MessagePoolDataStorage = segysdk::logger::MessagePool::MessagePoolDataStorage;
    static constexpr size_t messagesNumb_ = 20;
    static constexpr size_t threadsNumb_ = 4;
    static constexpr std::chrono::milliseconds sleepTime_ {10};
    static constexpr size_t consumeSize_ = 13;
    static constexpr size_t totalMessages_ = messagesNumb_ * threadsNumb_;
    static constexpr char delimiter_ = '.';

    void SetUp() override
    {
        generateMessages();
    }

    void TearDown() override
    {
        stopConsuming_ = true;
        writersPool_.joinAll();
        readersPool_.joinAll();
        stopConsuming_ = false;
        readMessages_.clear();
        messages_.clear();
        pool_.clear();
    }

    void startWriters()
    {
        auto addMessages = [this](size_t threadIdx) {
            for (const auto& message : messages_) {
                const auto pos = message.find(delimiter_);
                EXPECT_TRUE(pos != std::string::npos);
                if (threadIdx == std::stoul(message.substr(0, pos))) {
                    pool_.add(std::make_unique<segysdk::logger::Message>(message));
                }
            }
        };

        for (size_t i = 0; i < threadsNumb_; ++i) {
            writersPool_.addWorker(std::thread(addMessages, i));
        }
    }

    void startReaders()
    {
        auto consumeMessages = [this]() {
            while (!stopConsuming_.load()) {
                auto messages = pool_.consume_front(consumeSize_);
                if (!messages.empty()) {
                    std::lock_guard lock(readMutex_);
                    std::move(messages.begin(), messages.end(), std::back_inserter(readMessages_));
                }
            }
        };
        for (size_t i = 0; i < threadsNumb_; ++i) {
            readersPool_.addWorker(std::thread(consumeMessages));
        }
    }

    void checkReadMessages()
    {
        waitForReadMessages();
        std::lock_guard lock(readMutex_);
        EXPECT_EQ(readMessages_.size(), totalMessages_);
        for (auto& msg : readMessages_)
        {
            EXPECT_TRUE(std::find(messages_.begin(), messages_.end(), msg->message()) != messages_.end());
        }
    }

private:

    void waitForReadMessages()
    {
        // Make sure all writers have finished their jobs
        writersPool_.joinAll();
        size_t retries = 3;
        while (retries > 0) {
            {
                std::lock_guard lock(readMutex_);
                if (readMessages_.size() != totalMessages_) {
                    --retries;
                }
                else {
                    stopConsuming_ = true;
                    break;
                }
            }
            std::this_thread::sleep_for(sleepTime_);
        }
    }

    void generateMessages()
    {
        for (size_t threadIdx = 0; threadIdx < threadsNumb_; ++threadIdx) {
            for (size_t i = 0; i < messagesNumb_; ++i) {
                std::string message = std::to_string(threadIdx) + std::string(1, delimiter_) + std::string("Message number ") + std::to_string(i);
                messages_.emplace_back(std::move(message));
            }
        }
    }

private:
    std::mutex readMutex_;
    std::list<std::string> messages_;
    segysdk::logger::MessagePool pool_;
    std::atomic_bool stopConsuming_ = false;

    segysdk::concurrent::ThreadPool writersPool_ {threadsNumb_};
    segysdk::concurrent::ThreadPool readersPool_ {threadsNumb_};
    MessagePoolDataStorage readMessages_;
};


TEST_F(MessagePoolTest, WriteReadMultiple)
{
    startReaders();
    startWriters();
    checkReadMessages();
}

} // namespace segysdk::utilities::tests
