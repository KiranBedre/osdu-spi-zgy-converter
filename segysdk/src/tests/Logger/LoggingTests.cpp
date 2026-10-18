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

#include "../../Logger/Logger.h"
#include "../../Logger/LogWriterOstream.h"
#include "../../Utilities/GuidGenerator.h"

#include <gtest/gtest.h>

#include <sstream>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>

namespace segysdk::utilities::tests
{

class LogWriterCheck
{
public:
    LogWriterCheck() = default;
    virtual ~LogWriterCheck() = default;

    virtual void writeMessages(const segysdk::logger::LogWriterBase::PoolMessages& messages) = 0;
    virtual std::string getWrittenMessages() = 0;

    void writeMessagesWrapper(const segysdk::logger::LogWriterBase::PoolMessages& messages)
    {
        std::unique_lock lock(mtx_);
        writeMessages(messages);
        readyToRead_ = true;
        cv_.notify_one();
    }

    std::string getString(bool& timedOut)
    {
        timedOut = false;
        std::unique_lock lock(mtx_);
        if (cv_.wait_for(lock, timeoutDuration_, [this]() { return readyToRead_; })) {
            auto messages = getWrittenMessages();
            readyToRead_ = false;
            return messages;
        }
        timedOut = true;
        return {};
    }

    void setReadyToRead(bool readiness_)
    {
        readyToRead_ = readiness_;
    }

private:
    static constexpr auto timeoutDuration_ = std::chrono::milliseconds(10);
    std::mutex mtx_;
    std::condition_variable cv_;
    bool readyToRead_ = false;
};

class LogWriterMock : public segysdk::logger::LogWriterOstream,
                      public LogWriterCheck
{
public:
    LogWriterMock(std::shared_ptr<std::ostream> outStream)
      : LogWriterOstream(outStream), outStream_(outStream)
    {
    }

    virtual void writeMessages(const segysdk::logger::LogWriterBase::PoolMessages& messages) override
    {
        LogWriterOstream::write(messages);
    }

    virtual std::string getWrittenMessages() override
    {
        std::stringstream streamStr;
        streamStr << outStream_->rdbuf();
        return streamStr.str();
    }

    virtual void write(const segysdk::logger::LogWriterBase::PoolMessages& messages) override
    {
        writeMessagesWrapper(messages);
    }

private:
    // Same stream used for writing
    std::shared_ptr<std::ostream> outStream_;
};

class LogTest : public ::testing::Test
{
protected:
    static constexpr size_t writersSize_ = 4;
    static constexpr size_t messagesNumb_ = 1024;
    // Maximum number of timeouts that could be reached during the test
    // Timeout is reached when the message is not read
    static constexpr size_t timeoutsLimit_ = 1024;
    static constexpr segysdk::logger::LogLevel testLogLevel_ = segysdk::logger::LogLevel::Info;
    void SetUp() override
    {
        auto& logger = logger::Logger::getInstance();
        logger.setLogLevel(testLogLevel_);
        for (size_t i = 0; i < writersSize_; i++) {
            auto log_buffer = std::make_shared<std::stringstream>();
            writers_.push_back(std::make_shared<LogWriterMock>(log_buffer));
            logger.addLogWriterToDispatcher(writers_.back());
        }
        logger.runLogWriterDispatcher();
    }

    void TearDown () override
    {
        auto& logger = segysdk::logger::Logger::getInstance();
        logger.stopLogWriterDispatcher();
        writers_.clear();
    }

    std::shared_ptr<LogWriterCheck> getWriter(size_t i)
    {
        if (i > writersSize_) {
            return {};
        }
        return std::dynamic_pointer_cast<LogWriterCheck>(writers_[i]);
    }

    void checkWritersMessagesOrdered(const std::list<std::string>& messages)
    {
        for (size_t i = 0; i < writersSize_; ++i)
        {
            // collect all messages written by writer
            std::string full_msg = {};

            int timeoutsReached = 0;
            while (static_cast<std::size_t>(std::count(full_msg.begin(), full_msg.end(), '\n')) < messages.size())
            {
                bool timedOut = false;
                full_msg += getWriter(i)->getString(timedOut);
                if (timedOut) {
                    ++timeoutsReached;
                }
            }
            auto total = std::count(full_msg.begin(), full_msg.end(), '\n');

            EXPECT_LT(timeoutsReached, timeoutsLimit_);
            EXPECT_EQ(total, messages.size()) << i << " Writer has different number of messages";
            // compare
            size_t offset = 0;
            for (const auto& msg : messages)
            {
                size_t next_offset = full_msg.find(msg, offset);
                EXPECT_TRUE(next_offset != std::string::npos) << msg << " not found in messages";
                EXPECT_TRUE(next_offset > offset);
                offset = next_offset;
            }
        }
    }

    void checkWritersForNoMessage(const std::string& message)
    {
        for (size_t i = 0; i < writersSize_; ++i)
        {
            // No write was performed just mark the flag to unblock
            getWriter(i)->setReadyToRead(true);
            bool timedOut = false;
            std::string check = getWriter(i)->getString(timedOut);
            EXPECT_TRUE(check.find(message) == std::string::npos);
            EXPECT_FALSE(timedOut);
        }
    }

private:
    std::vector<std::shared_ptr<segysdk::logger::LogWriterBase>> writers_;
};

TEST_F(LogTest, LogOneMessage)
{
    using namespace segysdk::logger;
    std::string check_message = "Info message";
    SEGYLOG(LogTest::testLogLevel_, check_message);
    LogTest::checkWritersMessagesOrdered({check_message,});

    std::string no_message = "Should not be there";
    SEGYLOG(LogLevel::Debug, no_message);
    LogTest::checkWritersForNoMessage(no_message);
}

TEST_F(LogTest, LogSeveralAndClear)
{
    using namespace segysdk::logger;
    std::list<std::string> generated_messages;
    for (size_t i = 0; i < messagesNumb_; ++i)
    {
        generated_messages.emplace_back(GuidGenerator::newGuid());
    }
    for (auto& msg : generated_messages)
    {
        SEGYLOG(LogTest::testLogLevel_, msg);
    }

    LogTest::checkWritersMessagesOrdered(generated_messages);
}

} // namespace segysdk::utilities::tests
