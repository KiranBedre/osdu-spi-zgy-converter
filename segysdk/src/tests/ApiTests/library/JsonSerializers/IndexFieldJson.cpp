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

#include "IndexFieldJson.h"


namespace segysdk::api::tests
{

Json::Value IndexFieldJson::toJson(const segysdk::segyindex::IndexField& field)
{
    Json::Value json(Json::objectValue);
    json["segyFileNumber"] = field.getSegyFileNumber();
    json["inline"] = field.getInline();
    json["crossline"] = field.getCrossline();
    json["offset"] = Json::Value::UInt64(field.getOffset());
    return json;
}

} // namespace segysdk::api::tests
