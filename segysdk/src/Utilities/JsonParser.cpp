/*
Copyright 2025 Schlumberger

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

#include "JsonParser.h"

namespace segysdk
{

static auto
parse(const std::string& source)
{
    auto reader = Json::Reader{};
    auto result = Json::Value{};
    return std::pair{reader.parse(source, result, false), result};
}

Json::Value
parseFromString(const std::string& source)
{
    return parse(source).second;
}

Json::Value
parseFromStringRequired(const std::string& source)
{
    auto [success, result] = parse(source);
    if (!success) {
        throw JsonParseError{std::string{"failed to parse JSON: "}.append(source)};
    }
    return result;
}

JsonParseError::JsonParseError(const std::string& message)
  : runtime_error{message}
{
}

} // namespace segysdk
