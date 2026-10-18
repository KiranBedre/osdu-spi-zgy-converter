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

#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"

#include "../OsduMetaDataUtilities/OsduGenericFileDataLiterals.h"
#include "../OsduMetaDataUtilities/OsduTagElement.h"

namespace segysdk::metadata::osdu
{

OsduExtensionPropertyArray::OsduExtensionPropertyArray(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), extensionPropertyArrayStruct_(OsduExtensionPropertyArrayStruct(osduPartition)) {}

OsduExtensionPropertyArray::OsduExtensionPropertyArray(const OsduExtensionPropertyArray& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition()),
    extensionPropertyArrayStruct_(other.extensionPropertyArrayStruct_)
{
    setJson(other.getJson());
}

OsduExtensionPropertyArray::OsduExtensionPropertyArray(OsduExtensionPropertyArray&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition()),
    extensionPropertyArrayStruct_(std::move(other.extensionPropertyArrayStruct_))
{
    setJson(other.getJson());
}

OsduExtensionPropertyArray&
OsduExtensionPropertyArray::operator=(const OsduExtensionPropertyArray& rhs)
{
    return *this = OsduExtensionPropertyArray(rhs);
}

OsduExtensionPropertyArray&
OsduExtensionPropertyArray::operator=(OsduExtensionPropertyArray&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());

    extensionPropertyArrayStruct_ = std::move(rhs.extensionPropertyArrayStruct_);

    return *this;
}

OsduExtensionPropertyArray::OsduExtensionPropertyArray(OsduExtensionPropertyArrayStruct metaArray)
  : OsduMetadataBase(metaArray.getPartition()), extensionPropertyArrayStruct_(std::move(metaArray))
{
    populateJsonObject();
}

OsduExtensionPropertyArray::OsduExtensionPropertyArray(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), extensionPropertyArrayStruct_(OsduExtensionPropertyArrayStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduExtensionPropertyArray::isValidInput() const
{
    return extensionPropertyArrayStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduExtensionPropertyArray::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduExtensionPropertyArrayStruct>(extensionPropertyArrayStruct_);
}

void
OsduExtensionPropertyArray::initializeJson()
{
    assignRootJsonObject(Json::objectValue);
}

void
OsduExtensionPropertyArray::initializeMetadataStruct()
{
    extensionPropertyArrayStruct_.extensionPropertyArrayStruct_.clear();
}

void
OsduExtensionPropertyArray::populateMetadataStruct()
{
    // Extension properties is similar to tags, not an array but an object with unknown properties ...
    if (isMember(OsduGenericFileDataLiterals::ExtensionProperties)) {
        setJson(getParameter<Json::Value>(OsduGenericFileDataLiterals::ExtensionProperties));
    }

    std::vector<OsduExtensionPropertiesStruct> result;
    const auto memberList = getJsonMemberNames();
    for (const auto& element : memberList) {
        OsduExtensionPropertiesStruct extensionPropertyStruct(getOsduPartition());
        extensionPropertyStruct.propertyName = element;
        extensionPropertyStruct.propertyValue = getParameter<std::string>(element);
        result.emplace_back(extensionPropertyStruct);
    }

    extensionPropertyArrayStruct_.extensionPropertyArrayStruct_ = result;
}

void
OsduExtensionPropertyArray::populateJsonObject()
{
    initializeJson();
    // Extension properties is similar to tags, not an array but an object with unknown properties ...
    for (const auto& properties_struct : extensionPropertyArrayStruct_.extensionPropertyArrayStruct_) {
        setParameter(properties_struct.propertyName, properties_struct.propertyValue.has_value() ? properties_struct.propertyValue.value() : "");
    }
}

} // namespace segysdk::metadata::osdu
