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

#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeader.h"

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingStruct.h"
#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeaderStruct.h"
#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeaderLiterals.h"
#include "../OsduMetaDataUtilities/OsduOverrideByConstantValue.h"
#include "../OsduMetaDataUtilities/OsduOverrideByConstantValueStruct.h"

#include <algorithm>

namespace segysdk::metadata::osdu
{
OsduOverridesBinaryHeader::OsduOverridesBinaryHeader(std::string osduPartition)
    : OsduMetadataBase(std::move(osduPartition), true), vhmBhoStruct_(OsduOverridesBinaryHeaderStruct(getOsduPartition())) {}

OsduOverridesBinaryHeader::OsduOverridesBinaryHeader(const OsduOverridesBinaryHeader& other)
    : OsduMetadataBase(other.getOsduPartition(), true), vhmBhoStruct_(other.vhmBhoStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduOverridesBinaryHeader::OsduOverridesBinaryHeader(OsduOverridesBinaryHeader&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition(), true), vhmBhoStruct_(std::move(other.vhmBhoStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduOverridesBinaryHeader&
OsduOverridesBinaryHeader::operator=(const OsduOverridesBinaryHeader& rhs)
{
    return *this = OsduOverridesBinaryHeader(rhs);
}

OsduOverridesBinaryHeader&
OsduOverridesBinaryHeader::operator=(OsduOverridesBinaryHeader&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    vhmBhoStruct_ = std::move(rhs.vhmBhoStruct_);

    return *this;
}

OsduOverridesBinaryHeader::OsduOverridesBinaryHeader(OsduOverridesBinaryHeaderStruct binaryHeaderOverridesStruct)
    : OsduMetadataBase(binaryHeaderOverridesStruct.getPartition(), true), vhmBhoStruct_(std::move(binaryHeaderOverridesStruct))
{
    populateJsonObject();
}

OsduOverridesBinaryHeader::OsduOverridesBinaryHeader(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition, true), vhmBhoStruct_(OsduOverridesBinaryHeaderStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduOverridesBinaryHeader::isValidInput() const
{
    return vhmBhoStruct_.isValid();
}

void
OsduOverridesBinaryHeader::initializeJson()
{
    clearJsonObject();
    InitializeJsonRootAsArray();
}

void
OsduOverridesBinaryHeader::populateJsonObject()
{
    initializeJson();
    Json::Value binaryHeaderOverrides;
    if (vhmBhoStruct_.binaryHeaderOverrides_.has_value()) {
        for (const auto& bhOverride : vhmBhoStruct_.binaryHeaderOverrides_.value()) {
            const OsduOverrideByConstantValueStruct bho(bhOverride);
            const OsduOverrideByConstantValue cv(bho);
            const auto node = cv.getAsJson();
            if (!node.empty()) {
                appendRootArrayElement(node);
            }
        }
    }
}

void
OsduOverridesBinaryHeader::initializeMetadataStruct()
{
    vhmBhoStruct_ = OsduOverridesBinaryHeaderStruct(getOsduPartition());
}

void
OsduOverridesBinaryHeader::populateMetadataStruct()
{
    initializeMetadataStruct();
    if (isMember(OsduOverridesBinaryHeaderLiterals::BinaryHeaderOverrides)) {
        setJson(getParameter<Json::Value>(OsduOverridesBinaryHeaderLiterals::BinaryHeaderOverrides));
    }

    std::vector<OsduOverrideByConstantValueStruct> myOverrides;

    if (isRootJsonArray()) {
        for (const auto& node : getAllRootArrayElements<Json::Value>()) {
            OsduOverrideByConstantValue cv(node, getOsduPartition());
            const auto cvStruct = std::reinterpret_pointer_cast<OsduOverrideByConstantValueStruct>(cv.getMetaDataStruct());
            if (cvStruct != nullptr) {
                myOverrides.emplace_back(*cvStruct);
            }
        }
    }

    if (!myOverrides.empty()) {
        vhmBhoStruct_.binaryHeaderOverrides_ = myOverrides;
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduOverridesBinaryHeader::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduOverridesBinaryHeaderStruct>(vhmBhoStruct_);
}


} // namespace segysdk::metadata::osdu
