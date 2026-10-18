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

#include "../OsduMetaDataUtilities/OsduAbstractDatasetLiterals.h"
#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgy.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyData.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileCollectionOpenZgy::OsduFileCollectionOpenZgy(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileCollectionOpenZgyStruct_(OsduFileCollectionOpenZgyStruct(osduPartition))
{
    /* Empty */
}

OsduFileCollectionOpenZgy::OsduFileCollectionOpenZgy(OsduFileCollectionOpenZgyStruct fileCollectionOpenZgyStruct)
  : OsduMetadataBase(fileCollectionOpenZgyStruct.getPartition(), false), fileCollectionOpenZgyStruct_(std::move(fileCollectionOpenZgyStruct))
{
    populateJsonObject();
}

OsduFileCollectionOpenZgy::OsduFileCollectionOpenZgy(const OsduFileCollectionSegyStruct& FileCollectionSegyStruct)
  : OsduMetadataBase(FileCollectionSegyStruct.getPartition(), false), fileCollectionOpenZgyStruct_(FileCollectionSegyStruct.getPartition())
{
    populateStructFromFileCollectionSegy(FileCollectionSegyStruct);
    populateJsonObject();
}

OsduFileCollectionOpenZgy::OsduFileCollectionOpenZgy(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileCollectionOpenZgyStruct_(OsduFileCollectionOpenZgyStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileCollectionOpenZgy::isValidInput() const
{
    return fileCollectionOpenZgyStruct_.isValid();
}

void
OsduFileCollectionOpenZgy::initializeJson()
{
    clearJsonObject();
}

void
OsduFileCollectionOpenZgy::populateJsonObject()
{
    initializeJson();

    setStructParameter<OsduCommonMetadata>("", fileCollectionOpenZgyStruct_.commonMetadataStruct);
    if (fileCollectionOpenZgyStruct_.dataStruct.has_value()) {
        Json::Value additionJson = Json::objectValue;
        additionJson[OsduFileCollectionOpenZgyLiterals::Data] = OsduFileCollectionOpenZgyData(fileCollectionOpenZgyStruct_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduFileCollectionOpenZgy::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduFileCollectionOpenZgyLiterals::Id)) {
        const auto partition = OsduFileCollectionOpenZgyStruct::getPartitionFromReference(getParameter<std::string>(OsduFileCollectionOpenZgyLiterals::Id));
        setOsduPartition(partition);
    }

    fileCollectionOpenZgyStruct_ = OsduFileCollectionOpenZgyStruct(getOsduPartition());
}

void
OsduFileCollectionOpenZgy::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileCollectionOpenZgyStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    fileCollectionOpenZgyStruct_.dataStruct = getOptionalStructParameter<OsduFileCollectionOpenZgyData, OsduFileCollectionOpenZgyDataStruct>(OsduFileCollectionOpenZgyLiterals::Data);
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileCollectionOpenZgy::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileCollectionOpenZgyStruct>(fileCollectionOpenZgyStruct_);
}

void
OsduFileCollectionOpenZgy::populateStructFromFileCollectionSegy(const OsduFileCollectionSegyStruct& osduFileCollectionSegyStruct)
{
    // ToDo: Hard-code to version 1.0.0 until ADME upgrades to more recent version
    fileCollectionOpenZgyStruct_.commonMetadataStruct.kind = OsduFileCollectionOpenZgyStruct::getOsduKind();
    fileCollectionOpenZgyStruct_.commonMetadataStruct.aclStruct = osduFileCollectionSegyStruct.commonMetadataStruct.aclStruct;
    fileCollectionOpenZgyStruct_.commonMetadataStruct.legalStruct = osduFileCollectionSegyStruct.commonMetadataStruct.legalStruct;
    fileCollectionOpenZgyStruct_.commonMetadataStruct.tagsStruct = osduFileCollectionSegyStruct.commonMetadataStruct.tagsStruct;
    fileCollectionOpenZgyStruct_.commonMetadataStruct.metaStruct = osduFileCollectionSegyStruct.commonMetadataStruct.metaStruct;

    const std::string osduPartition = getOsduPartition();

    if (osduFileCollectionSegyStruct.commonMetadataStruct.id.has_value() and osduFileCollectionSegyStruct.commonMetadataStruct.version.has_value()) {
        OsduAncestryStruct ancestry(osduPartition);

        const auto parent = OsduMetadataHelper::osduReferenceLookupString(osduFileCollectionSegyStruct.commonMetadataStruct.id.value(), osduFileCollectionSegyStruct.commonMetadataStruct.version.value());
        if (!parent.empty()) {
            ancestry.parents = {parent};
        }
        fileCollectionOpenZgyStruct_.commonMetadataStruct.ancestryStruct = ancestry;
    }

    OsduFileCollectionOpenZgyDataStruct dataStruct(osduPartition);
    OsduAbstractCommonResourcesStruct commonResources(osduPartition);
    OsduAbstractDatasetStruct datasetStruct(osduPartition);

    OsduMetadataHelper helper(osduPartition);
    commonResources.existenceKind = helper.osduReferenceLookupString(segysdk::metadata::osdu::OsduFileCollectionOpenZgyLiterals::ExistenceKindReference, segysdk::metadata::osdu::OsduFileCollectionOpenZgyLiterals::ExistenceActual);
    datasetStruct.endian = segysdk::metadata::osdu::OsduAbstractDatasetLiterals::LittleEndian;
 
    if (osduFileCollectionSegyStruct.dataStruct.has_value()) {
        if (osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.has_value()) {
            commonResources.resourceHomeRegionID = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().resourceHomeRegionID;
            commonResources.resourceHostRegionIDs = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().resourceHostRegionIDs;
            commonResources.resourceCurationStatus = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().resourceCurationStatus;
            commonResources.resourceLifecycleStatus = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().resourceLifecycleStatus;
            commonResources.resourceSecurityClassification = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().resourceSecurityClassification;
            commonResources.source = osduFileCollectionSegyStruct.dataStruct.value().commonResourcesStruct.value().source;
        }
        dataStruct.commonResourcesStruct = commonResources;

        if (osduFileCollectionSegyStruct.dataStruct.value().datasetStruct.has_value()) {
            
            datasetStruct.description = osduFileCollectionSegyStruct.dataStruct.value().datasetStruct.value().description;
            datasetStruct.name = osduFileCollectionSegyStruct.dataStruct.value().datasetStruct.value().name;
            dataStruct.datasetStruct = datasetStruct;
        }

        if (osduFileCollectionSegyStruct.dataStruct.value().fileCollectionStruct.has_value()) {
            OsduAbstractFileCollectionStruct fileCollectionStruct(osduPartition);
            fileCollectionStruct.fileCollectionPath = osduFileCollectionSegyStruct.dataStruct.value().fileCollectionStruct.value().fileCollectionPath;
            fileCollectionStruct.fileSourceInfoStructs = std::vector<OsduAbstractFileSourceInfoStruct>();
            dataStruct.fileCollectionStruct = fileCollectionStruct;
        }
    }

    fileCollectionOpenZgyStruct_.dataStruct = dataStruct;
}

} // namespace segysdk::metadata::osdu
