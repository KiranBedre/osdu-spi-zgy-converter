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

#include "CrsConverterSvcClient.h"

#include "../Logger/Logger.h"
#include "../Utilities/Environment.h"
#include "../Utilities/Settings.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/auth/HttpCredentialProvider.h"

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
CrsConverterSvcClient::addHeaders(web::http::http_request& request, std::vector<segysdk::utilities::auth::HttpCredentialProvider::Header>& additional_headers, std::string& data_partition_id)
{
    SEGYLOG_DEBUG("Adding Http headers");
    for (const auto& header : segysdk::utilities::Settings::instance().getHttpHeaders()) {
        SEGYLOG_DEBUG("[" + header.first + "] : [" + header.second + "]\n");
        request.headers().add(conversions::to_string_t(header.first), conversions::to_string_t(header.second));
    }

    for (const auto& h : additional_headers) {
        request.headers().add(conversions::to_string_t(h.name), conversions::to_string_t(h.value));
    }
    const auto dataPartitionId = U("data-partition-id");
    if (request.headers().find(dataPartitionId) == request.headers().end()) {
        request.headers().add(dataPartitionId, conversions::to_string_t(data_partition_id));
    }

    const auto contentType = U("Content-type");
    if (request.headers().find(contentType) != request.headers().end()) {
        request.headers().remove(contentType);
    }

    const auto applicationJson = U("application/json");
    request.headers().add(contentType, applicationJson);

    const auto accept = U("Accept");
    request.headers().add(accept, U("*/*"));
}

CrsConverterSvcClient::CrsConverterSvcClient(std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider> credentialProvider)
  : credentialProvider_(std::move(credentialProvider))
{
}
std::string
CrsConverterSvcClient::getToConvertJsonBody(const Json::Value& asIngested)
{
    Json::Value ConvertMe;
    ConvertMe["featureCollection"] = asIngested;
    ConvertMe["toCRS"] = wgs84crs;

    return ConvertMe.toStyledString();
}

std::string
CrsConverterSvcClient::convertRecord(const Json::Value& geoJson) const
{
    if (geoJson.empty()) {
        // Return null Json object if id is empty
        return "{}";
    }
    auto service_url = segysdk::utilities::Environment::instance().sdUrl();
    auto data_partition_id = segysdk::utilities::Environment::instance().osduDataPartitionId();
    auto additional_headers = credentialProvider_->headers();

    auto pos = service_url.find("seistore-svc");
    if (pos == std::string::npos) {
        return "{}";
    }
    const auto crsGeoJsonConverterUrl = service_url.substr(0, pos) + "api/crs/converter/v2";

    SEGYLOG_DEBUG("CRS conversion service URL: [" + crsGeoJsonConverterUrl + "]");

    SEGYLOG_DEBUG("Data partition ID  : [" + data_partition_id + "]");
    if (segysdk::utilities::Environment::instance().insecurePrintTokenToDebug()) {
        SEGYLOG_DEBUG("Auth headers       : [" + std::to_string(additional_headers.size()) + "]");
        for (const auto& h : additional_headers) {
            SEGYLOG_DEBUG("    " + h.name + ": " + h.value);
        }
    }

    uri_builder crs_converter_uri_builder(conversions::to_string_t(crsGeoJsonConverterUrl));
    crs_converter_uri_builder.append(U("convertGeoJson"));
    auto crs_converter_uri(crs_converter_uri_builder.to_uri());

   // SEGYLOG_DEBUG("POST [" + conversions::to_utf8string(crs_converter_uri.to_string()) + "]");

    http_client client(crs_converter_uri);
    http_request request;
    request.set_method(methods::POST);
    addHeaders(request, additional_headers, data_partition_id);

    const auto convertJson = getToConvertJsonBody(geoJson);
    request.set_body(convertJson);

    const auto cUtl_uri = crs_converter_uri.to_string();
    const auto cUrl_request = request.to_string();

    auto response = client.request(request).get();

    auto status_code = response.status_code();
    SEGYLOG_DEBUG("Status code " + std::to_string(status_code));
    auto result_content = response.extract_utf8string().get();
    SEGYLOG_DEBUG("Got " + std::to_string(result_content.size()) + " chars of content");

    if (status_code == status_codes::OK) {
        return result_content;
    }

    SEGYLOG_DEBUG("Server response: [" + result_content + "]");
    return "{}";
}

} // namespace segysdk::utilities
