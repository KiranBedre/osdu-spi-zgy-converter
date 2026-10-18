/*
Copyright 2021-2022 Schlumberger

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

#include "StorageSvcClient.h"

#include "JsonUtilities.h"

#include "../Logger/Logger.h"
#include "../Utilities/Environment.h"
#include "../Utilities/Settings.h"
#include "../Utilities/auth/HttpCredentialProvider.h"
#include "../Utilities/osdu/OsduObjectReference.h"

#include <cpprest/http_client.h>
#include <json/json.h>

using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace http;                 // Common HTTP functionality
using namespace client;               // HTTP client features
using namespace concurrency::streams; // Asynchronous streams

namespace segysdk::utilities
{

void
addHeaders(web::http::http_request& request, std::vector<segysdk::utilities::auth::HttpCredentialProvider::Header>& additional_headers, std::string& data_partition_id)
{
    SEGYLOG_DEBUG("Adding Http headers");
    for (auto header : utilities::Settings::instance().getHttpHeaders()) {
        SEGYLOG_DEBUG("[" + header.first + "] : [" + header.second + "]\n");
        request.headers().add(conversions::to_string_t(header.first), conversions::to_string_t(header.second));
    }

    for (const auto& h : additional_headers) {
        request.headers().add(conversions::to_string_t(h.name), conversions::to_string_t(h.value));
    }

    if (request.headers().find(U("data-partition-id")) == request.headers().end()) {
        request.headers().add(U("data-partition-id"), conversions::to_string_t(data_partition_id));
    }

    if (request.headers().find(U("Content-type")) == request.headers().end()) {
        request.headers().add(U("Content-type"), U("application/json"));
    }
}

StorageSvcClient::StorageSvcClient(std::unique_ptr<auth::HttpCredentialProvider> credentialProvider)
  : credentialProvider_(std::move(credentialProvider))
{
}

std::string
StorageSvcClient::getRecord(const std::string& recordId) const
{
    if (recordId.empty()) {
        // Return null Json object if id is empty
        return "{}";
    }
    auto service_url = Environment::instance().storageSvcUrl();
    auto data_partition_id = Environment::instance().osduDataPartitionId();
    auto additional_headers = credentialProvider_->headers();

    SEGYLOG_DEBUG("About to get record [" + recordId + "]");
    SEGYLOG_DEBUG("Storage service URL: [" + service_url + "]");
    SEGYLOG_DEBUG("Data partition ID  : [" + data_partition_id + "]");
    if (Environment::instance().insecurePrintTokenToDebug()) {
        SEGYLOG_DEBUG("Auth headers       : [" + std::to_string(additional_headers.size()) + "]");
        for (const auto& h : additional_headers) {
            SEGYLOG_DEBUG("    " + h.name + ": " + h.value);
        }
    }

    const auto ref = osdu::OsduObjectReference(recordId);
    SEGYLOG_DEBUG("About to get record [" + recordId + "] version " + std::to_string(ref.specifiedVersion()));

    uri_builder service_uri_builder(conversions::to_string_t(service_url));
    service_uri_builder.append(U("records"));
    service_uri_builder.append(uri::encode_data_string(conversions::to_string_t(ref.objectId())));
    if (!ref.isLatestVersion()) {
        service_uri_builder.append(conversions::to_string_t(std::to_string(ref.specifiedVersion())));
    }
    auto service_uri(service_uri_builder.to_uri());

    SEGYLOG_DEBUG("GET [" + conversions::to_utf8string(service_uri.to_string()) + "]");

    http_client client(service_uri);
    http_request request;
    request.set_method(methods::GET);

    addHeaders(request, additional_headers, data_partition_id);

    try {
        auto response = client.request(request).get();

        auto status_code = response.status_code();
        SEGYLOG_DEBUG("Status code " + std::to_string(status_code));
        auto result_content = response.extract_utf8string().get();
        SEGYLOG_DEBUG("Got " + std::to_string(result_content.size()) + " chars of content");

        if (status_code == status_codes::OK) {
            return result_content;
        }

        SEGYLOG_DEBUG("Server response: [" + result_content + "]");
        throw std::runtime_error("Storage Service returned status code " + std::to_string(status_code));
    }
    catch (const std::exception& e) {
        std::string msg = "StorageSvcClient::getRecord: Exception caught" + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);

        throw;
    }
}

Json::Value
StorageSvcClient::getRecordAsJson(const std::string& recordId) const
{
    return JsonUtilities::getJsonFromString(getRecord(recordId));
}

std::string
StorageSvcClient::storeRecord(const std::string& recordContent) const
{
    auto storage_svc_url = Environment::instance().storageSvcUrl();
    auto data_partition_id = Environment::instance().osduDataPartitionId();
    auto additional_headers = credentialProvider_->headers();

    SEGYLOG_DEBUG("About to PUT record [" + std::to_string(recordContent.size()) + " chars]");
    SEGYLOG_DEBUG("Storage service URL: [" + storage_svc_url + "]");
    SEGYLOG_DEBUG("Data partition ID  : [" + data_partition_id + "]");
    if (Environment::instance().insecurePrintTokenToDebug()) {
        SEGYLOG_DEBUG("Auth headers       : [" + std::to_string(additional_headers.size()) + "]");
        for (const auto& h : additional_headers) {
            SEGYLOG_DEBUG("    " + h.name + ": " + h.value);
        }
    }
    uri_builder service_uri_builder(conversions::to_string_t(storage_svc_url));
    service_uri_builder.append(U("records"));
    auto service_uri(service_uri_builder.to_uri());

    SEGYLOG_DEBUG("PUT [" + conversions::to_utf8string(service_uri.to_string()) + "]");

    http_client client(service_uri);
    http_request request;
    request.set_method(methods::PUT);

    addHeaders(request, additional_headers, data_partition_id);

    request.set_body("[" + recordContent + "]");

    try {
        auto response = client.request(request).get();

        auto status_code = response.status_code();
        SEGYLOG_DEBUG("Status code " + std::to_string(status_code));
        auto result_content = response.extract_utf8string().get();
        SEGYLOG_DEBUG("Got " + std::to_string(result_content.size()) + " chars of content");

        if (status_code == status_codes::OK || status_code == status_codes::Created || status_code == status_codes::Accepted) {
            const auto result = JsonUtilities::getJsonFromString(result_content);
            if (!result.isMember("recordCount") || !result.isMember("recordIds") || result["recordCount"].asInt() != 1
                || !result["recordIds"].isArray() || result["recordIds"].size() != 1 || result["recordIds"][0] == "") {
                throw std::runtime_error("Failed to parse Storage Service response.");
            }
            auto recordId = result["recordIds"][0].asString();
            SEGYLOG_DEBUG("Stored " + recordId + " Using storage service");
            SEGYLOG_GSM_INFO("storeRecord: " + recordId);
            return recordId;
        }
        else {
            SEGYLOG_REPORT_ERROR("Server response: [" + result_content + "]");
        }
        throw std::runtime_error("Storage Service returned status code " + std::to_string(status_code));
    }
    catch (const std::exception& e) {
        std::string msg = "StorageSvcClient::storeRecord: Exception caught" + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);

        throw;
    }
}

} // namespace segysdk::utilities
