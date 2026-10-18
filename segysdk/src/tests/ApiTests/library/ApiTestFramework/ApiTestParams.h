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

#include "ApiTestResult.h"
#include "ApiTestException.h"
#include "../JsonSerializers/JsonDeserializer.h"

#include <Utilities/Environment.h>

#include <json/json.h>
#include <filesystem>
#include <optional>

namespace segysdk::api::tests
{
class ApiTestParams
{
public:
    ApiTestParams(const Json::Value& data)
      : parameters_(data["parameters"])
    {
    }

    template<typename T>
    T getParameter(const std::string& name) const
    {
        const auto& parameter = parameters_[name];
        if (parameter.isNull()) {
            throw ApiTestException(("Cannot find parameter: " + name).c_str());
        }
        return JsonDeserializer::fromJson<T>(parameter);
    }

    template<typename T>
    std::optional<T> getOptionalParameter(const std::string& name) const
    {
        const auto& parameter = parameters_[name];
        if (parameter.isNull()) {
            return std::nullopt;
        }
        return JsonDeserializer::fromJson<T>(parameter);
    }

private:
    Json::Value parameters_;
};
} // namespace segysdk::api::tests
