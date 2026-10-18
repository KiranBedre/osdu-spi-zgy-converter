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

#include "HeaderDataStructJson.h"

namespace segysdk::api::tests
{

Json::Value
HeaderDataStructJson::toJson(const segysdk::api::HeaderDataStruct& headerData)
{
    Json::Value json(Json::objectValue);
    json["id"] = headerData.id;
    json["offset"] = Json::Value::UInt64(headerData.offset);
    json["end"] = Json::Value::UInt64(headerData.end);
    json["value"] = headerData.value;
    return json;
}

} // namespace segysdk::api::tests
