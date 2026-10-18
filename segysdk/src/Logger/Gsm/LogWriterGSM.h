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

#include "../../segysdk_details/DLL_EXPORT.h"

#include "../LogWriterBase.h"
#include "../LoggerSettings.h"
#include "../MessageGSM.h"

#include "../../Utilities/auth/HttpCredentialProvider.h"

#include <string>
#include <ostream>
#include <memory>
#include <map>

namespace segysdk::logger
{

class SEGYSDK_DLL_PUBLIC EnvironmentGsm
{
public:
    static std::string GSM_PUBLISHER_URL();
    static std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider> acquireGsmToken();

private:
    class EnvironmentVar
    {
    public:
        inline static const char* GSM_PUBLISHER_URL = "GSM_PUBLISHER_URL";
    };
};

class SEGYSDK_DLL_PUBLIC LogWriterGSM : public segysdk::logger::LogWriterBase
{
public:
    using PoolGsmMessages = std::list<std::shared_ptr<segysdk::logger::MessageGSM>>;

    LogWriterGSM(std::string correlationId, std::string recordId, const std::map<std::string, std::string>& requestHeaders);
    virtual ~LogWriterGSM() override;

    virtual void write(const segysdk::logger::LogWriterBase::PoolMessages& messages) override;

private:
    std::string messageToStatus(const MessageGSM& message);

    void sendMessages(const PoolGsmMessages& messages);

    std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider> credentialProvider_;
    const std::string publisherUrl_;
    const std::string correlationId_;
    const std::string recordId_;
    const std::map<std::string, std::string> requestHeaders_;
};

} // namespace internal::segysdk

using namespace segysdk::logger;

extern "C" {
    SEGYSDK_DLL_PUBLIC LogWriterBase* createLogWriter(const LoggerSettings* settings);

    SEGYSDK_DLL_PUBLIC void deleteLogWriter(LogWriterBase* writer);
}
