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

#include "../OsduMetaDataUtilities/OsduExtensionProperties.h"

#include "../OsduMetaDataUtilities/OsduExtensionPropertiesLiterals.h"

namespace segysdk::metadata::osdu
{
OsduExtensionProperties::OsduExtensionProperties(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), extensionPropertiesStruct_(OsduExtensionPropertiesStruct(osduPartition)) {}

OsduExtensionProperties::OsduExtensionProperties(const OsduExtensionProperties& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      extensionPropertiesStruct_(other.extensionPropertiesStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduExtensionProperties::OsduExtensionProperties(OsduExtensionProperties&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      extensionPropertiesStruct_(std::move(other.extensionPropertiesStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduExtensionProperties&
OsduExtensionProperties::operator=(const OsduExtensionProperties& rhs)
{
    return *this = OsduExtensionProperties(rhs);
}

OsduExtensionProperties&
OsduExtensionProperties::operator=(OsduExtensionProperties&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    extensionPropertiesStruct_ = std::move(rhs.extensionPropertiesStruct_);

    return *this;
}

OsduExtensionProperties::OsduExtensionProperties(OsduExtensionPropertiesStruct extensionStruct)
    : OsduMetadataBase(extensionStruct.getPartition()), extensionPropertiesStruct_(std::move(extensionStruct))
{
    populateJsonObject();
}

OsduExtensionProperties::OsduExtensionProperties(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), extensionPropertiesStruct_(OsduExtensionPropertiesStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduExtensionProperties::isValidInput() const
{
    return extensionPropertiesStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduExtensionProperties::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduExtensionPropertiesStruct>(extensionPropertiesStruct_);
}

void
OsduExtensionProperties::initializeJson()
{
    removeMember(OsduExtensionPropertiesLiterals::PropertyName);
    removeMember(OsduExtensionPropertiesLiterals::PropertyValue);
}

void
OsduExtensionProperties::populateJsonObject()
{
    initializeJson();
    setParameter(OsduExtensionPropertiesLiterals::PropertyName, extensionPropertiesStruct_.propertyName);
    setOptionalParameter(OsduExtensionPropertiesLiterals::PropertyValue, extensionPropertiesStruct_.propertyValue);
}

void
OsduExtensionProperties::initializeMetadataStruct()
{
    extensionPropertiesStruct_ = OsduExtensionPropertiesStruct(getOsduPartition());
}

void
OsduExtensionProperties::populateMetadataStruct()
{
    initializeMetadataStruct();

    extensionPropertiesStruct_.propertyName = getParameter<std::string>(OsduExtensionPropertiesLiterals::PropertyName);
    extensionPropertiesStruct_.propertyValue = getOptionalParameter<std::string>(OsduExtensionPropertiesLiterals::PropertyValue);
}

} // namespace segysdk::metadata::osdu
