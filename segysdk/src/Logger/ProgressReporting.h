/*
Copyright 2023 Schlumberger

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

#include "../Concurrent/ThreadPool.h"
#include "../Logger/MessageGSM.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>

namespace segysdk::logger
{

class SEGYSDK_DLL_PUBLIC ProgressReporting
{
private:
    static constexpr std::chrono::seconds sleepTime_{5};

public:
    ProgressReporting(std::chrono::seconds freq, std::string task);

    ProgressReporting(const ProgressReporting&) = delete;
    ProgressReporting(ProgressReporting&&) = delete;
    ProgressReporting& operator=(const ProgressReporting&) = delete;
    ProgressReporting& operator=(ProgressReporting&&) = delete;

    ~ProgressReporting();

    std::chrono::seconds getFrequency() const;
    std::string getTaskName() const;

    void report(std::string message, double percentComplete);
    void setProgressPrecision(double precision) { progressPrecision_ = precision; }

private:
    void reportMessageToGsm();
    std::chrono::seconds getSleepTime() const;

private:
    std::chrono::seconds reportFrequency_{};
    std::string taskName_;
    std::chrono::time_point<std::chrono::steady_clock> reportedTime_{};
    ProgressMessageGSM reportedMessage_;
    std::mutex reportMessageMtx_;

    std::atomic_bool exitReporterThread_{false};
    segysdk::concurrent::ThreadPool reporter_;
    double lastProgressReported_;
    double progressPrecision_;
};

} // namespace segysdk::logger
