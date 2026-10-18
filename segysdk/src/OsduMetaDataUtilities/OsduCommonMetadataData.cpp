/*
Copyright 2024 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduCommonMetadataData.h"

#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadataDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduCommonMetadataData::OsduCommonMetadataData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    commonMetadataDataStruct_(OsduCommonMetadataDataStruct(osduPartition))
{
    /* Empty */
}

OsduCommonMetadataData::OsduCommonMetadataData(OsduCommonMetadataDataStruct commonMetadataDataStruct)
  : OsduMetadataBase(commonMetadataDataStruct.getPartition()), commonMetadataDataStruct_(std::move(commonMetadataDataStruct))
{
    populateJsonObject();
}

OsduCommonMetadataData::OsduCommonMetadataData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), commonMetadataDataStruct_(OsduCommonMetadataDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduCommonMetadataData::isValidInput() const
{
    return commonMetadataDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduCommonMetadataData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduCommonMetadataDataStruct>(commonMetadataDataStruct_);
}

void
OsduCommonMetadataData::initializeJson()
{
    clearJsonObject();
}

void
OsduCommonMetadataData::populateJsonObject()
{
    initializeJson();

    const auto extensionProperties = OsduExtensionPropertyArray(commonMetadataDataStruct_.extensionPropertiesStruct);
    setParameter(OsduCommonMetadataDataLiterals::ExtensionProperties, extensionProperties.getAsJson());

}

void
OsduCommonMetadataData::initializeMetadataStruct()
{
    commonMetadataDataStruct_ = OsduCommonMetadataDataStruct(getOsduPartition());
}

void
OsduCommonMetadataData::populateMetadataStruct()
{
    initializeMetadataStruct();

    commonMetadataDataStruct_.extensionPropertiesStruct = getStructParameter<OsduExtensionPropertyArray, OsduExtensionPropertyArrayStruct>(OsduCommonMetadataDataLiterals::ExtensionProperties);
}
} // namespace segysdk::metadata::osdu
