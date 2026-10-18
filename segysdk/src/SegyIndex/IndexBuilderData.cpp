/*
Copyright 2022 Schlumberger

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

#include "IndexBuilderData.h"

#include <json/json.h>

namespace segysdk::segyindex
{

Json::Value
IndexBuilderData::toJsonValue() const
{
    Json::Value root;

    root[fieldInlineNo_] = InlineNo;
    root[fieldCrosslineNo_] = CrosslineNo;
    root[fieldCMP_] = CMP;
    root[fieldTraceNo_] = static_cast<Json::UInt64>(TraceNo);
    root[fieldFileNo_] = static_cast <Json::UInt64>(FileNo);
    root[fieldOffset_] = static_cast <Json::UInt64>(Offset);
    root[fieldSampleCount_] = static_cast <Json::UInt64>(SampleCount);
    root[fieldX_] = X;
    root[fieldY_] = Y;

    return root;
}

std::string
IndexBuilderData::toJsonString() const
{
    Json::Value root = toJsonValue();

    return root.toStyledString().c_str();
}

} // namespace segysdk::segyindex
