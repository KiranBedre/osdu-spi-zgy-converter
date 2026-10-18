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

#pragma once

#include "../FileIO/GenericFileReader.h"

#include <json/json.h>

#include <memory>
#include <string>

namespace SampleCode::Common
{
struct JsonToolkit
{
private:
    static bool isEmptyString(const std::string& str);

public:
    static bool isValidJson(const std::string& json_string);
    static std::shared_ptr<segysdk::fileio::GenericFileReader> CreateGenericFileReader(const std::string& fileName);

    static std::string GetJsonFromFile(const std::string& osduFileCollectionJson);
    static std::string GetJsonFromFile(const std::shared_ptr<segysdk::fileio::GenericFileReader>& reader);

    static Json::Value getJsonFromString(const std::string& json_string);
    static Json::Value ParseJsonString(const std::string& json_string);

    static bool StringMatches(const std::string& str, const std::string& match);
};

} // namespace SampleCode::Common
