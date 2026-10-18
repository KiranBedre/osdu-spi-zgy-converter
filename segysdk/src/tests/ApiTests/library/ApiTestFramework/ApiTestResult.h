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

#include "ApiTestException.h"

#include <json/json.h>
#include <filesystem>

namespace segysdk::api::tests
{
enum class ResultType {
    R_SUCCESS,
    R_VALUE,
    R_VECTOR,
    R_OBJECT,
    R_FILE,
    R_TEXT,
    R_JSON,
    R_JSON_FILE,
    R_EXCEPTION
};

class ApiTestResult
{
public:
    ApiTestResult(const Json::Value& testDataJson);
    ~ApiTestResult(){};

    ResultType getType() const;
    Json::Value getValue() const;
    std::string getExceptionMsg() const;
    std::filesystem::path getExpectedFile() const;
    std::filesystem::path getBaselineFile() const;
    std::filesystem::path getBaselineTextFilePath() const;
    std::filesystem::path getBaselineJsonFilePath() const;

private:
    ResultType type_;
    Json::Value value_;
    std::string exceptionMsg_;
    std::string expectedFilePath_;
    std::string baselineFilePath_;
    std::string baselineTextFilePath_;
    std::string baselineJsonFilePath_;
};
} // namespace segysdk::api::tests
