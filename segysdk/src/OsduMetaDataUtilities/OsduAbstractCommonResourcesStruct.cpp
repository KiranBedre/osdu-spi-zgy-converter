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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractCommonResourcesStruct::OsduAbstractCommonResourcesStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
     {}

OsduAbstractCommonResourcesStruct::OsduAbstractCommonResourcesStruct(const OsduAbstractCommonResourcesStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    resourceHomeRegionID(other.resourceHomeRegionID),
    resourceHostRegionIDs(other.resourceHostRegionIDs),
    resourceCurationStatus(other.resourceCurationStatus),
    resourceLifecycleStatus(other.resourceLifecycleStatus),
    resourceSecurityClassification(other.resourceSecurityClassification),
    source(other.source),
    existenceKind(other.existenceKind)
{
    /* Empty */
}

OsduAbstractCommonResourcesStruct::OsduAbstractCommonResourcesStruct(OsduAbstractCommonResourcesStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    resourceHomeRegionID(std::move(other.resourceHomeRegionID)),
    resourceHostRegionIDs(std::move(other.resourceHostRegionIDs)),
    resourceCurationStatus(std::move(other.resourceCurationStatus)),
    resourceLifecycleStatus(std::move(other.resourceLifecycleStatus)),
    resourceSecurityClassification(std::move(other.resourceSecurityClassification)),
    source(std::move(other.source)),
    existenceKind(std::move(other.existenceKind))
{
    /* Empty */
}

OsduAbstractCommonResourcesStruct&
OsduAbstractCommonResourcesStruct::operator=(const OsduAbstractCommonResourcesStruct& rhs)
{
    return *this = OsduAbstractCommonResourcesStruct(rhs);
}

OsduAbstractCommonResourcesStruct&
OsduAbstractCommonResourcesStruct::operator=(OsduAbstractCommonResourcesStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    resourceHomeRegionID = std::move(rhs.resourceHomeRegionID);
    resourceHostRegionIDs = std::move(rhs.resourceHostRegionIDs);
    resourceCurationStatus = std::move(rhs.resourceCurationStatus);
    resourceLifecycleStatus = std::move(rhs.resourceLifecycleStatus);
    resourceSecurityClassification = std::move(rhs.resourceSecurityClassification);
    source = std::move(rhs.source);
    existenceKind = std::move(rhs.existenceKind);

    return *this;
}

bool
OsduAbstractCommonResourcesStruct::isValid() const
{
    return isValidIdString(resourceHomeRegionID, IdStringCheckType::Reference) and isValidIdStringArray(resourceHostRegionIDs, IdStringCheckType::Reference)
           and isValidIdString(resourceCurationStatus, IdStringCheckType::Reference) and isValidIdString(resourceLifecycleStatus, IdStringCheckType::Reference)
           and isValidIdString(resourceSecurityClassification, IdStringCheckType::Reference) and isValidIdString(existenceKind, IdStringCheckType::Reference);
}

std::vector<std::string>
OsduAbstractCommonResourcesStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    
    std::vector<std::string> results;
    results.emplace_back("OsduAbstractCommonResourcesStruct::isValid() failed.");

    performIdStringValidityScan(results, resourceHomeRegionID, "OsduAbstractCommonResourcesStruct::resourceHomeRegionID", IdStringCheckType::Reference);
    performIdStringArrayValidityScan(results, resourceHostRegionIDs, "OsduAbstractCommonResourcesStruct::resourceHostRegionIDs", IdStringCheckType::Reference);
    performIdStringValidityScan(results, resourceCurationStatus, "OsduAbstractCommonResourcesStruct::resourceCurationStatus", IdStringCheckType::Reference);
    performIdStringValidityScan(results, resourceLifecycleStatus, "OsduAbstractCommonResourcesStruct::resourceLifecycleStatus", IdStringCheckType::Reference);
    performIdStringValidityScan(results, resourceSecurityClassification, "OsduAbstractCommonResourcesStruct::resourceSecurityClassification", IdStringCheckType::Reference);
    performIdStringValidityScan(results, existenceKind, "OsduAbstractCommonResourcesStruct::existenceKind", IdStringCheckType::Reference);

    return results;
}
} // namespace segysdk::metadata::osdu
