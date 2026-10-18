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

#include "../Logger/Message.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>

#include <chrono>
#include <string>

namespace segysdk::logger
{

namespace StatusDetails
{
inline static const char* const correlationId = "correlationId";
inline static const char* const recordId = "recordId";
inline static const char* const recordIdVersion = "recordIdVersion";
inline static const char* const stage = "stage";
inline static const char* const status = "status";
inline static const char* const message = "message";
inline static const char* const errorCode = "errorCode";
inline static const char* const additionalProperties = "additionalProperties";

namespace Properties
{
inline static const char* const taskName = "taskName";
inline static const char* const taskProgress = "taskProgress";
} // namespace Properties

namespace Stage
{
inline static const char* const ingestorSync = "INGESTOR_SYNC";
} // namespace Stage

namespace Status
{
inline static const char* const inProgress = "IN_PROGRESS";
}

} // namespace StatusDetails

class SEGYSDK_DLL_PUBLIC MessageGSM : public Message
{
public:
    explicit MessageGSM(std::string message = "");

    MessageGSM(MessageGSM const& other) = default;
    MessageGSM& operator=(MessageGSM const& other) = default;
    MessageGSM(MessageGSM&& other) = default;
    MessageGSM& operator=(MessageGSM&& other) = default;

    virtual ~MessageGSM() override = default;

    virtual Json::Value getGsmMessage() const;
};

class SEGYSDK_DLL_PUBLIC ProgressMessageGSM : public MessageGSM
{
public:
    explicit ProgressMessageGSM(std::string message = "");
    ProgressMessageGSM(std::string message, std::string taskName, double taskProgress);

    ProgressMessageGSM(ProgressMessageGSM const& other) = default;
    ProgressMessageGSM& operator=(ProgressMessageGSM const& other) = default;
    ProgressMessageGSM(ProgressMessageGSM&& other) = default;
    ProgressMessageGSM& operator=(ProgressMessageGSM&& other) = default;

    virtual ~ProgressMessageGSM() override = default;

    Json::Value getGsmMessage() const override;

    std::string taskName() const;
    double taskProgress() const;

private:
    std::string taskName_ {};
    double taskProgress_ {};
};

} // namespace segysdk::logger
