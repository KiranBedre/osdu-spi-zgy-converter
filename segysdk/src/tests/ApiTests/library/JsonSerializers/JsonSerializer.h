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

#include <map>
#include <functional>
#include <algorithm>

#include <json/json.h>

namespace segysdk::api::tests
{
class JsonSerializer
{
public:
    template <typename Serializer, typename T>
    static Json::Value toJson(const std::string& memberName, const T& value)
    {
        Json::Value json;
        json[memberName] = toJson<Serializer>(value);
        return json;
    }

    template <typename T>
    static Json::Value toJson(const std::string& memberName, const T& value)
    {
        Json::Value json;
        json[memberName] = toJson(value);
        return json;
    }

    template <typename Serializer, typename T>
    static Json::Value toJson(const T& value)
    {
        return Serializer::toJson(value);
    }

    // NOTE: 3 functions below produce Json::Value objects that don't produce a valid JSON file when written to.
    // They are made public to be used in different serializers
    template <typename T>
    static Json::Value toJson(const T& value)
    {
        return Json::Value(value);
    }

    template <typename Serializer, typename T>
    static Json::Value toJson(const std::vector<T>& array)
    {
        Json::Value jsonArray(Json::arrayValue);
        std::for_each(array.begin(), array.end(), [&jsonArray](const T& value) { jsonArray.append(toJson<Serializer>(value)); });
        return jsonArray;
    }

    template <typename T>
    static Json::Value toJson(const std::vector<T>& array)
    {
        Json::Value jsonArray(Json::arrayValue);
        std::for_each(array.begin(), array.end(), [&jsonArray](const T& value) { jsonArray.append(toJson(value)); });
        return jsonArray;
    }
};

} // namespace segysdk::api::tests

