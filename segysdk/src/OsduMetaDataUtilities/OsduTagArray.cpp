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

#include "../OsduMetaDataUtilities/OsduTagArray.h"

#include "../OsduMetaDataUtilities/OsduGenericFileLiterals.h"
#include "../OsduMetaDataUtilities/OsduTagElement.h"
#include "../OsduMetaDataUtilities/OsduTagLiterals.h"

namespace segysdk::metadata::osdu
{


OsduTagArray::OsduTagArray(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), tagArrayStruct_(OsduTagArrayStruct(osduPartition)) {}

OsduTagArray::OsduTagArray(const OsduTagArray& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      tagArrayStruct_(other.tagArrayStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduTagArray::OsduTagArray(OsduTagArray&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      tagArrayStruct_(std::move(other.tagArrayStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduTagArray&
OsduTagArray::operator=(const OsduTagArray& rhs)
{
    return *this = OsduTagArray(rhs);
}

OsduTagArray&
OsduTagArray::operator=(OsduTagArray&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    tagArrayStruct_ = std::move(rhs.tagArrayStruct_);

    return *this;
}

OsduTagArray::OsduTagArray(OsduTagArrayStruct metaArray)
    : OsduMetadataBase(metaArray.getPartition()), tagArrayStruct_(std::move(metaArray))
{
    populateJsonObject();
}

OsduTagArray::OsduTagArray(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), tagArrayStruct_(OsduTagArrayStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduTagArray::isValidInput() const
{
    return tagArrayStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduTagArray::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduTagArrayStruct>(tagArrayStruct_);
}

void
OsduTagArray::initializeJson()
{
    Json::Value root;
    root = Json::objectValue;
    assignRootJsonObject(root);
}

void
OsduTagArray::initializeMetadataStruct()
{
    tagArrayStruct_.tagStructs_ = std::nullopt;
}

void
OsduTagArray::populateMetadataStruct()
{
    initializeMetadataStruct();

    // We don't know parameter names here so simply process the entire member list
    const auto memberList = getJsonMemberNames();
    if (memberList.empty()) {      
        return;
    }

    std::vector<OsduTagElementStruct> result;

    for (const auto& name : memberList) {
        OsduTagElementStruct tagStruct(getOsduPartition());
        tagStruct.keyName = name;
        tagStruct.keyValue = getParameter<std::string>(name);

        result.emplace_back(tagStruct);
    }

    tagArrayStruct_.tagStructs_ = result;
}

void
OsduTagArray::populateJsonObject()
{
    initializeJson();
    if (tagArrayStruct_.tagStructs_.has_value()) {
        for (const auto& m : tagArrayStruct_.tagStructs_.value()) {
            setParameter(m.keyName, m.keyValue);
        }
    }
}

} // namespace segysdk::metadata::osdu
