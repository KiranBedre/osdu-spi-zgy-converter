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

#include "ApiTestResult.h"
#include "ApiTestUtilities.h"

namespace segysdk::api::tests
{
ApiTestResult::ApiTestResult(const Json::Value& testDataJson)
{
    static const std::map<std::string, ResultType> resultMap = {
        {"success", ResultType::R_SUCCESS},
        {"value", ResultType::R_VALUE},
        {"vector", ResultType::R_VECTOR},
        {"object", ResultType::R_OBJECT},
        {"file", ResultType::R_FILE},
        {"text", ResultType::R_TEXT},
        {"json", ResultType::R_JSON},
        {"json_file", ResultType::R_JSON_FILE},
        {"throws", ResultType::R_EXCEPTION}};

    if (testDataJson.isNull()) {
        throw ApiTestException("Test result is not specified.");
    }

    const auto& resultJson = testDataJson["result"];

    auto iter = resultMap.find(resultJson.asString());
    if (iter == resultMap.end()) {
        throw ApiTestException("Unknown test result.");
    }

    type_ = iter->second;

    switch (type_) {
    case ResultType::R_SUCCESS:
        {
            break;
        }
    case ResultType::R_VALUE:
        {
            value_ = testDataJson["value"];
            break;
        }
    case ResultType::R_VECTOR:
        {
            baselineFilePath_ = testDataJson["baseline"].asString();
            break;
        }
    case ResultType::R_OBJECT:
        {
            break;
        }
    case ResultType::R_FILE:
        {
            expectedFilePath_ = testDataJson["file"].asString();
            baselineFilePath_ = testDataJson["baseline"].asString();
            break;
        }
    case ResultType::R_TEXT:
        {
            baselineTextFilePath_ = testDataJson["textfile"].asString();
            break;
        }
    case ResultType::R_JSON:
        {
            baselineJsonFilePath_ = testDataJson["jsonfile"].asString();
            break;
        }
    case ResultType::R_JSON_FILE:
        {
            expectedFilePath_ = testDataJson["file"].asString();
            baselineJsonFilePath_ = testDataJson["jsonfile"].asString();
            break;
        }
    case ResultType::R_EXCEPTION:
        {
            exceptionMsg_ = testDataJson["exception"].asString();
            break;
        }
    }
}

ResultType
ApiTestResult::getType() const
{
    return type_;
}

Json::Value
ApiTestResult::getValue() const
{
    if (type_ != ResultType::R_VALUE) {
        throw ApiTestException("Test is not expected to return a value");
    }

    return value_;
}

std::string
ApiTestResult::getExceptionMsg() const
{
    if (type_ != ResultType::R_EXCEPTION) {
        throw ApiTestException("Test is not expected to throw an exception");
    }

    return exceptionMsg_;
}

std::filesystem::path
ApiTestResult::getExpectedFile() const
{
    if (type_ != ResultType::R_FILE && type_ != ResultType::R_JSON_FILE) {
        throw ApiTestException("Test is not expected to generate a file");
    }

    // TODO: At the moment most it appears that generated files are located alongside dataset
    return getTestdataPath(expectedFilePath_);
}

std::filesystem::path
ApiTestResult::getBaselineFile() const
{
    if (type_ != ResultType::R_FILE && type_ != ResultType::R_VECTOR) {
        throw ApiTestException("Test is not expected to generate a file");
    }

    return getTestdataPath(baselineFilePath_);
}

std::filesystem::path
ApiTestResult::getBaselineTextFilePath() const
{
    if (type_ != ResultType::R_TEXT) {
        throw ApiTestException("Test is not expected to generate text results");
    }

    return getTestdataPath(baselineTextFilePath_);
}

std::filesystem::path
ApiTestResult::getBaselineJsonFilePath() const
{
    if (type_ != ResultType::R_JSON && type_ != ResultType::R_JSON_FILE) {
        throw ApiTestException("Test is not expected to generate JSON results");
    }

    return getTestdataPath(baselineJsonFilePath_);
}
} // namespace segysdk::api::tests
