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

#include "LogWriterGSM.h"

#include "../../Utilities/Authorization.h"
#include "../../Utilities/Environment.h"
#include "../../Utilities/EnvironmentVariables.h"
#include "../../Utilities/GuidGenerator.h"
#include "../../Utilities/JsonUtilities.h"
#include "../../Utilities/Settings.h"
#include "../../Utilities/TokenSourceFactory.h"
#include "../../Utilities/auth/BearerTokenAuth.h"
#include "../Logger.h"

#include <cpprest/http_client.h>

#include <iostream>

namespace segysdk::logger
{

std::string
EnvironmentGsm::GSM_PUBLISHER_URL()
{
    return segysdk::utilities::EnvironmentVariable::getStringEnv(EnvironmentVar::GSM_PUBLISHER_URL);
}

std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider>
EnvironmentGsm::acquireGsmToken()
{
    // Reuse what we have already.
    // @TODO: Should we have for each service it's unique token
    return std::make_unique<segysdk::utilities::auth::BearerTokenAuth>(
        segysdk::utilities::TokenSourceFactory::getTokenSource());
}

LogWriterGSM::LogWriterGSM(std::string correlationId, std::string recordId, const std::map<std::string, std::string>& requestHeaders)
  : credentialProvider_(std::move(EnvironmentGsm::acquireGsmToken())),
    publisherUrl_(EnvironmentGsm::GSM_PUBLISHER_URL()),
    correlationId_(correlationId),
    recordId_(recordId),
    requestHeaders_(requestHeaders)
{
}

LogWriterGSM::~LogWriterGSM()
{
    stopWriter();
}

void
LogWriterGSM::sendMessages(const PoolGsmMessages& messages)
{
    web::uri_builder service_uri_builder(utility::conversions::to_string_t(publisherUrl_));
    service_uri_builder.append(U("v1"));
    service_uri_builder.append(U("status"));
    auto service_uri(service_uri_builder.to_uri());

    auto client = web::http::client::http_client(service_uri);

    web::http::http_request request;
    request.set_method(web::http::methods::POST);
    for (const auto& headers : requestHeaders_) {
        request.headers().add(utility::conversions::to_string_t(headers.first),
                              utility::conversions::to_string_t(headers.second));
    }
    for (const auto& h : credentialProvider_->headers()) {
        request.headers().add(utility::conversions::to_string_t(h.name), utility::conversions::to_string_t(h.value));
    }
    request.headers().add(U("Content-type"), U("application/json"));

    std::string whole_message;
    for (auto msg_it = messages.cbegin(); msg_it != messages.cend(); msg_it++) {
        whole_message += messageToStatus(**msg_it);
        if (std::next(msg_it, 1) != messages.end()) {
            whole_message += ",";
        }
    }
    request.set_body("[" + whole_message + "]");

    try {
        auto response = client.request(request).get();
        auto status_code = response.status_code();

        if (status_code != web::http::status_codes::OK) {
            const auto reason = response.extract_utf8string().get();
            std::cerr << "Server response: [" + reason + "]" << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "GSM: exception during request: " << ex.what() << std::endl;
    }
}

void
LogWriterGSM::write(const segysdk::logger::LogWriterBase::PoolMessages& messages)
{
    if (not messages.empty()) {
        PoolGsmMessages gsmMessages;
        std::for_each(messages.begin(), messages.end(),
                      [&gsmMessages](auto& msg) {
                          if (msg->type() == MessageType::GSM) {
                              gsmMessages.push_back(std::dynamic_pointer_cast<MessageGSM>(msg));
                          }
                      });
        if (not gsmMessages.empty()) {
            sendMessages(gsmMessages);
        }
    }
}

std::string
LogWriterGSM::messageToStatus(const MessageGSM& message)
{
    Json::Value root = message.getGsmMessage();

    // update message
    root[StatusDetails::correlationId] = correlationId_;
    root[StatusDetails::recordId] = recordId_;
    root[StatusDetails::stage] = StatusDetails::Stage::ingestorSync;
    root[StatusDetails::status] = StatusDetails::Status::inProgress;
    // @TODO: Should here be some error Code?
    root[StatusDetails::errorCode] = 0;

    return root.toStyledString();
}

} // namespace segysdk::logger

segysdk::logger::LogWriterBase*
createLogWriter(const segysdk::logger::LoggerSettings* settings)
{
    if (segysdk::logger::EnvironmentGsm::GSM_PUBLISHER_URL().empty() || settings->getCorrelationId().empty() || settings->getFileCollectionId().empty()) {
        return nullptr;
    }

    // Need to copy the headers as Settings is global object and access to it is not synchronized
    auto headers = segysdk::utilities::Settings::instance().getHttpHeaders();
    auto writer_p = std::make_unique<segysdk::logger::LogWriterGSM>(settings->getCorrelationId(), settings->getFileCollectionId(), headers);
    return writer_p.release();
}

void
deleteLogWriter(segysdk::logger::LogWriterBase* writer)
{
    if (writer) {
        delete writer;
    }
}
