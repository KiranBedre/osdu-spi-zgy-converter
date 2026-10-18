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

#include "../OsduMetaDataUtilities/OsduOverridesTraceHeader.h"

#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderElement.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderStruct.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderLiterals.h"

#include <algorithm>
#include <utility>


namespace segysdk::metadata::osdu
{
OsduOverridesTraceHeader::OsduOverridesTraceHeader(std::string osduPartition)
    : OsduMetadataBase(std::move(osduPartition), true), traceHeaderOverridesStruct_(OsduOverridesTraceHeaderStruct(getOsduPartition()))
{
    /* Empty */
}

OsduOverridesTraceHeader::OsduOverridesTraceHeader(const OsduOverridesTraceHeader& other)
    : OsduMetadataBase(other.getOsduPartition(), true), traceHeaderOverridesStruct_(other.traceHeaderOverridesStruct_)
{
    setJson(other.getJson());
}

OsduOverridesTraceHeader::OsduOverridesTraceHeader(OsduOverridesTraceHeader&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), traceHeaderOverridesStruct_(std::move(other.traceHeaderOverridesStruct_))
{
    setJson(other.getJson());
}

OsduOverridesTraceHeader&
OsduOverridesTraceHeader::operator=(const OsduOverridesTraceHeader& rhs)
{
    return *this = OsduOverridesTraceHeader(rhs);
}

OsduOverridesTraceHeader&
OsduOverridesTraceHeader::operator=(OsduOverridesTraceHeader&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    traceHeaderOverridesStruct_ = std::move(rhs.traceHeaderOverridesStruct_);

    return *this;
}

OsduOverridesTraceHeader::OsduOverridesTraceHeader(OsduOverridesTraceHeaderStruct thoStruct)
    : OsduMetadataBase(thoStruct.getPartition()), traceHeaderOverridesStruct_(std::move(thoStruct))
{
    populateJsonObject();
}

OsduOverridesTraceHeader::OsduOverridesTraceHeader(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), traceHeaderOverridesStruct_(OsduOverridesTraceHeaderStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduOverridesTraceHeader::isValidInput() const
{
    return traceHeaderOverridesStruct_.isValid();
}

void
OsduOverridesTraceHeader::initializeJson()
{
    clearJsonObject();
}

void
OsduOverridesTraceHeader::populateJsonObject()
{
    initializeJson();
    if (traceHeaderOverridesStruct_.traceHeaderOverrides_.has_value()) {
        for (const auto& node : traceHeaderOverridesStruct_.traceHeaderOverrides_.value()) {
            OsduOverridesTraceHeaderElement element(node);
            appendRootArrayElement(element.getAsJson());
        }
    }
}

void
OsduOverridesTraceHeader::initializeMetadataStruct()
{
    traceHeaderOverridesStruct_ = OsduOverridesTraceHeaderStruct(getOsduPartition());
}

void
OsduOverridesTraceHeader::populateMetadataStruct()
{
    initializeMetadataStruct();

    std::vector<OsduOverridesTraceHeaderElementStruct> myOverrides;
    if (isMember(OsduOverridesTraceHeaderLiterals::TraceHeaderOverrides)) {
        setJson(getParameter<Json::Value>(OsduOverridesTraceHeaderLiterals::TraceHeaderOverrides));
    }

    if (isRootJsonArray()) {
        for (const auto& node : getAllRootArrayElements<Json::Value>()) {
            OsduOverridesTraceHeaderElement thoElement(node, getOsduPartition());
            const auto thoStruct = std::reinterpret_pointer_cast<OsduOverridesTraceHeaderElementStruct>(thoElement.getMetaDataStruct());
            if (thoStruct != nullptr) {
                myOverrides.emplace_back(*thoStruct);
            }
        }
    }

    if (!myOverrides.empty()) {
        traceHeaderOverridesStruct_.traceHeaderOverrides_ = myOverrides;
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduOverridesTraceHeader::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduOverridesTraceHeaderStruct>(traceHeaderOverridesStruct_);
}

} // namespace segysdk::metadata::osdu
