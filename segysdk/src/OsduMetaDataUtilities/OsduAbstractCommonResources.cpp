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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractCommonResources::OsduAbstractCommonResources(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    commonResourcesStruct_(OsduAbstractCommonResourcesStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractCommonResources::OsduAbstractCommonResources(OsduAbstractCommonResourcesStruct commonResourceStruct)
    : OsduMetadataBase(commonResourceStruct.getPartition()), commonResourcesStruct_(std::move(commonResourceStruct))
{
    populateJsonObject();
}


OsduAbstractCommonResources::OsduAbstractCommonResources(Json::Value json, const std::string& osduPartition)
    : OsduMetadataBase(std::move(json), osduPartition),
      commonResourcesStruct_(OsduAbstractCommonResourcesStruct(osduPartition))
{
    populateMetadataStruct();
}

OsduAbstractCommonResources::OsduAbstractCommonResources(const OsduAbstractCommonResources& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      commonResourcesStruct_(other.commonResourcesStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractCommonResources::OsduAbstractCommonResources(OsduAbstractCommonResources&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      commonResourcesStruct_(std::move(other.commonResourcesStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractCommonResources&
OsduAbstractCommonResources::operator=(const OsduAbstractCommonResources& rhs)
{
    return *this = OsduAbstractCommonResources(rhs);
}

OsduAbstractCommonResources&
OsduAbstractCommonResources::operator=(OsduAbstractCommonResources&& rhs) noexcept
{
    commonResourcesStruct_ = std::move( rhs.commonResourcesStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    return *this;
}

bool
OsduAbstractCommonResources::isValidInput() const
{
    return commonResourcesStruct_.isValid();
}


void
OsduAbstractCommonResources::initializeJson()
{
    removeMember(OsduAbstractCommonResourcesLiterals::ExistenceKind);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification);
    removeMember(OsduAbstractCommonResourcesLiterals::Source);
}

void
OsduAbstractCommonResources::populateJsonObject()
{
    // Do NOT clean Json object - it is in OsduDataElement space ...
    initializeJson();
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::ExistenceKind, commonResourcesStruct_.existenceKind);
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID, commonResourcesStruct_.resourceHomeRegionID);
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus, commonResourcesStruct_.resourceCurationStatus);
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus, commonResourcesStruct_.resourceLifecycleStatus);
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification, commonResourcesStruct_.resourceSecurityClassification);
    setOptionalParameter(OsduAbstractCommonResourcesLiterals::Source, commonResourcesStruct_.source);
    if (commonResourcesStruct_.resourceHostRegionIDs.has_value()) {
        for (const auto& country : commonResourcesStruct_.resourceHostRegionIDs.value()) {
            appendArrayElement(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs, country);
        }
    }
    
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractCommonResources::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractCommonResourcesStruct>(commonResourcesStruct_);
}

void
OsduAbstractCommonResources::initializeMetadataStruct()
{
    commonResourcesStruct_ = OsduAbstractCommonResourcesStruct(getOsduPartition());
}

void
OsduAbstractCommonResources::populateMetadataStruct()
{
    initializeMetadataStruct();

    commonResourcesStruct_.existenceKind = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::ExistenceKind);
    commonResourcesStruct_.resourceHomeRegionID = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID);
    if (isJsonArray(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs)) {
        std::vector<std::string> myArray;
        for (const auto& country : getAllArrayElements<std::string>(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs)) {
            myArray.emplace_back(country);
        }
        commonResourcesStruct_.resourceHostRegionIDs = myArray;
    }

    commonResourcesStruct_.resourceCurationStatus = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus);
    commonResourcesStruct_.resourceLifecycleStatus = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus);
    commonResourcesStruct_.resourceSecurityClassification = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification);
    commonResourcesStruct_.source = getOptionalParameter<std::string>(OsduAbstractCommonResourcesLiterals::Source);
}

} // namespace segysdk::metadata::osdu
