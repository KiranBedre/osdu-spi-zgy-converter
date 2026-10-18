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

#include <utility>

#include "../OsduMetaDataUtilities/OsduTagElement.h"

#include "../OsduMetaDataUtilities/OsduTagLiterals.h"

namespace segysdk::metadata::osdu
{

OsduTagElement::OsduTagElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), tagStruct_(OsduTagElementStruct(osduPartition))
{
    /* Empty */
}

OsduTagElement::OsduTagElement(const OsduTagElement& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      tagStruct_(other.tagStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduTagElement::OsduTagElement(OsduTagElement&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      tagStruct_(std::move(other.tagStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduTagElement&
OsduTagElement::operator=(const OsduTagElement& rhs)
{
    return *this = OsduTagElement(rhs);
}

OsduTagElement&
OsduTagElement::operator=(OsduTagElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    tagStruct_ = std::move(rhs.tagStruct_);

    return *this;
}

OsduTagElement::OsduTagElement(OsduTagElementStruct tagElementStruct)
    : OsduMetadataBase(tagElementStruct.getPartition(), true), tagStruct_(std::move(tagElementStruct))
{
    populateJsonObject();
}

OsduTagElement::OsduTagElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), tagStruct_(OsduTagElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduTagElement::isValidInput() const
{
    return tagStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduTagElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduTagElementStruct>(tagStruct_);
}

void
OsduTagElement::initializeJson()
{
    clearJsonObject();
}

void
OsduTagElement::populateJsonObject()
{
    initializeJson();
    if (!tagStruct_.isValid()) {
        return;
    }

    setParameter(OsduTagLiterals::KeyName, tagStruct_.keyName);
    setParameter(OsduTagLiterals::KeyValue, tagStruct_.keyValue);
}

void
OsduTagElement::initializeMetadataStruct() {}

void
OsduTagElement::populateMetadataStruct()
{

    tagStruct_.keyName = getParameter<std::string>(OsduTagLiterals::KeyName);
    tagStruct_.keyValue = getParameter<std::string>(OsduTagLiterals::KeyValue);
}

} // namespace segysdk::metadata::osdu
