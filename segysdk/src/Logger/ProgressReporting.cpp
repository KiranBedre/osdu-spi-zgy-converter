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

#include "../Logger/ProgressReporting.h"
#include "../Logger/Logger.h"
#include "../Utilities/Environment.h"
#include <sstream>

namespace segysdk::logger
{

ProgressReporting::ProgressReporting(std::chrono::seconds freq, std::string task)
  : reportFrequency_(freq), taskName_(task), reporter_(1), lastProgressReported_(-1), progressPrecision_(utilities::Environment::instance().progressPrecision())
{
    reporter_.addWorker(std::thread(&ProgressReporting::reportMessageToGsm, this));
}

ProgressReporting::~ProgressReporting()
{
    // finish progress reporting
    if (reportedMessage_.taskProgress() < 100.0) {
        report("Task is finished", 100.0);
    }
    exitReporterThread_ = true;
}

std::chrono::seconds
ProgressReporting::getFrequency() const
{
    return reportFrequency_;
}

std::string
ProgressReporting::getTaskName() const
{
    return taskName_;
}

std::chrono::seconds
ProgressReporting::getSleepTime() const
{
    return getFrequency() < sleepTime_ ? getFrequency() : sleepTime_;
}

void
ProgressReporting::reportMessageToGsm()
{
    std::chrono::time_point<std::chrono::steady_clock> lastReportedTime{};
    double lastReportedPercent = -1;

    auto reportFunc = [this, &lastReportedTime, &lastReportedPercent](bool isLast) {
        std::unique_lock lock(reportMessageMtx_);
        if (reportedTime_ != lastReportedTime and (isLast or reportedTime_ - lastReportedTime > getFrequency())) {
            if (not isLast and lastReportedPercent > -1) {
                std::stringstream updatedMsg{reportedMessage_.taskName(), std::ios_base::app | std::ios_base::out};
                updatedMsg << ":" << reportedMessage_.message();
                updatedMsg << "; progress: " << reportedMessage_.taskProgress() << "%";
                reportedMessage_.updateMessage(updatedMsg.str());
            }
            SEGYLOG_GSM_INFO(reportedMessage_);
            lastReportedTime = reportedTime_;
            lastReportedPercent = reportedMessage_.taskProgress();
        }
    };

    while (not exitReporterThread_.load()) {
        reportFunc(false);
        std::this_thread::sleep_for(getSleepTime());
    }
    reportFunc(true);
}

void
ProgressReporting::report(std::string message, double percentComplete)
{
    if (percentComplete < lastProgressReported_ + progressPrecision_) {
        return;
    }
    else {
        lastProgressReported_ = percentComplete;
    }

    if (percentComplete > 100 or percentComplete < 0) {
        SEGYLOG_DEBUG("Reporting percent is not in [0, 100] range");
        return;
    }
    if (percentComplete < reportedMessage_.taskProgress()) {
        std::stringstream warningMsg;
        warningMsg << "Reported percent:" << percentComplete << " is less than reported previously: " << reportedMessage_.taskProgress();
        SEGYLOG_WARNING(warningMsg);
        return;
    }
    std::unique_lock lock(reportMessageMtx_);
    reportedMessage_ = ProgressMessageGSM(message, getTaskName(), percentComplete);
    reportedTime_ = std::chrono::steady_clock::now();
}

} // namespace segysdk::logger
