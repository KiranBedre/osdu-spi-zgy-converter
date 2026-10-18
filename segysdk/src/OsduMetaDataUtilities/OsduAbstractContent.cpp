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

#include "../OsduMetaDataUtilities/OsduAbstractContent.h"

#include "../OsduMetaDataUtilities/OsduExtensionProperties.h"
#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"
#include "../OsduMetaDataUtilities/OsduAbstractContentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractContentLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractContent::OsduAbstractContent(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), abstractContentStruct_(OsduAbstractContentStruct(osduPartition)) {}

OsduAbstractContent::OsduAbstractContent(const OsduAbstractContent& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractContentStruct_(other.abstractContentStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractContent::OsduAbstractContent(OsduAbstractContent&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractContentStruct_(std::move(other.abstractContentStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractContent&
OsduAbstractContent::operator=(const OsduAbstractContent& rhs)
{
    return *this = OsduAbstractContent(rhs);
}

OsduAbstractContent&
OsduAbstractContent::operator=(OsduAbstractContent&& rhs) noexcept
{
    abstractContentStruct_ = std::move(rhs.abstractContentStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    return *this;
}

OsduAbstractContent::OsduAbstractContent(OsduAbstractContentStruct abstractContentStruct)
    : OsduMetadataBase(abstractContentStruct.getPartition()), abstractContentStruct_(std::move(abstractContentStruct))
{
    populateJsonObject();
}

OsduAbstractContent::OsduAbstractContent(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), abstractContentStruct_(OsduAbstractContentStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractContent::isValidInput() const
{
    return abstractContentStruct_.isValid();
}

void
OsduAbstractContent::initializeJson()
{
    removeMember(OsduAbstractContentLiterals::SchemaKind);
    removeMember(OsduAbstractContentLiterals::Name);
    removeMember(OsduAbstractContentLiterals::Description);
    removeMember(OsduAbstractContentLiterals::ExtensionProperties);
}

std::optional<Json::Value>
OsduAbstractContent::getExtensionPropertiesJson() const
{
    if (!abstractContentStruct_.extensionProperties.has_value()) {
        return std::nullopt;
    }
    const OsduExtensionPropertyArray ep(abstractContentStruct_.extensionProperties.value());

    return ep.getAsJson();
}

void
OsduAbstractContent::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduAbstractContentLiterals::SchemaKind, abstractContentStruct_.schemaKind);
    setOptionalParameter(OsduAbstractContentLiterals::Name, abstractContentStruct_.name);
    setOptionalParameter(OsduAbstractContentLiterals::Description, abstractContentStruct_.description);
    setOptionalParameter(OsduAbstractContentLiterals::ExtensionProperties, getExtensionPropertiesJson());
}

void
OsduAbstractContent::initializeMetadataStruct()
{
    abstractContentStruct_ = OsduAbstractContentStruct(getOsduPartition());
}

void
OsduAbstractContent::populateMetadataStruct()
{
    initializeMetadataStruct();
    abstractContentStruct_.schemaKind = getParameter<std::string>(OsduAbstractContentLiterals::SchemaKind);
    abstractContentStruct_.name = getOptionalParameter<std::string>(OsduAbstractContentLiterals::Name);
    abstractContentStruct_.description = getOptionalParameter<std::string>(OsduAbstractContentLiterals::Description);

    const auto epJson = getOptionalParameter<Json::Value>(OsduAbstractContentLiterals::ExtensionProperties);
    if (epJson.has_value()) {
        OsduExtensionPropertyArray epArray(epJson.value(), getOsduPartition());
        const auto epArrayStructPtr = std::reinterpret_pointer_cast<OsduExtensionPropertyArrayStruct>(epArray.getMetaDataStruct());
        abstractContentStruct_.extensionProperties = *epArrayStructPtr;
    } 
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractContent::getMetaDataStruct()
{
    populateMetadataStruct();
    const auto ptr = std::make_shared<OsduAbstractContentStruct>(abstractContentStruct_);
    return ptr;
}

} // namespace segysdk::metadata::osdu
