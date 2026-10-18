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

#include "../common/TypeChecks.h"
#include "JsonException.h"

#include <json/json.h>

#include <algorithm>

namespace segysdk::api::tests
{
class JsonDeserializer
{
public:
    template<typename T>
    static T fromJson(const Json::Value& json)
    {
        if constexpr (std::is_same_v<T, Json::Value>) {
            return json;
        }
        if constexpr (std::is_same_v<T, bool>) {
            return json.asBool();
        }
        else if constexpr (std::is_unsigned_v<T> || std::is_enum_v<T>) {
            return json.asUInt64();
        }
        else if constexpr (std::is_integral_v<T>) {
            return json.asInt64();
        }
        else if constexpr (std::is_floating_point_v<T>) {
            return json.asDouble();
        }
        else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, std::string>) {
            return json.asString();
        }
        else if constexpr (is_std_vector<T>::value) {
            if (!json.isArray()) {
                throw JsonException("Expecting JSON array");
            }

            T array;
            std::for_each(json.begin(), json.end(),
                          [&array](const Json::Value& value) { array.emplace_back(fromJson<typename T::value_type>(value)); });
            return array;
        }
        else {
            throw JsonException((std::string("Unsupported parameter type: ") + typeid(T).name()).c_str());
        }
    }

    template<typename Deserializer, typename T>
    static T fromJson(const Json::Value& json)
    {
        if constexpr (is_std_vector<T>::value) {
            if (!json.isArray()) {
                throw JsonException("Expecting JSON array");
            }

            T array;
            std::for_each(json.begin(), json.end(),
                          [&array](const Json::Value& value) { array.emplace_back(fromJson<Deserializer, typename T::value_type>(value)); });
            return array;
        }
        else {
            return Deserializer::fromJson(json);
        }
    }
};
} // namespace segysdk::api::tests