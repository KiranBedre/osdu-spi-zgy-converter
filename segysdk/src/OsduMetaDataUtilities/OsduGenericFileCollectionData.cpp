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

#include "../OsduMetaDataUtilities/OsduGenericFileCollectionData.h"

#include "../OsduMetaDataUtilities/OsduAbstractFileCollectionLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesLiterals.h"
#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"
#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionDataStruct.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionDataLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileCollection.h"

namespace segysdk::metadata::osdu
{
OsduGenericFileCollectionData::OsduGenericFileCollectionData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileDataStruct_(OsduGenericFileCollectionDataStruct(osduPartition)) {}

OsduGenericFileCollectionData::OsduGenericFileCollectionData(const OsduGenericFileCollectionData& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    fileDataStruct_(other.fileDataStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileCollectionData::OsduGenericFileCollectionData(OsduGenericFileCollectionData&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    fileDataStruct_(std::move(other.fileDataStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileCollectionData&
OsduGenericFileCollectionData::operator=(const OsduGenericFileCollectionData& rhs)
{
    return *this = OsduGenericFileCollectionData(rhs);
}

OsduGenericFileCollectionData&
OsduGenericFileCollectionData::operator=(OsduGenericFileCollectionData&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    fileDataStruct_ = std::move(rhs.fileDataStruct_);

    return *this;
}

OsduGenericFileCollectionData::OsduGenericFileCollectionData(OsduGenericFileCollectionDataStruct fileDataStruct)
  : OsduMetadataBase(fileDataStruct.getPartition(), false), fileDataStruct_(std::move(fileDataStruct))
{
    populateJsonObject();
}

OsduGenericFileCollectionData::OsduGenericFileCollectionData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileDataStruct_(OsduGenericFileCollectionDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGenericFileCollectionData::isValidInput() const
{
    return fileDataStruct_.isValid();
}

void
OsduGenericFileCollectionData::initializeJson()
{
    clearJsonObject();
}

void
OsduGenericFileCollectionData::populateJsonObject()
{
    initializeJson();

    if (fileDataStruct_.abstractCommonResourcesStruct.has_value()) {
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ExistenceKind, fileDataStruct_.abstractCommonResourcesStruct.value().existenceKind);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus, fileDataStruct_.abstractCommonResourcesStruct.value().resourceCurationStatus);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification, fileDataStruct_.abstractCommonResourcesStruct.value().resourceSecurityClassification);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus, fileDataStruct_.abstractCommonResourcesStruct.value().resourceLifecycleStatus);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::Source, fileDataStruct_.abstractCommonResourcesStruct.value().source);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID, fileDataStruct_.abstractCommonResourcesStruct.value().resourceHomeRegionID);
        if (fileDataStruct_.abstractCommonResourcesStruct->resourceHostRegionIDs.has_value()) {
            InitializeJsonArray(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs);
            for (const auto& regionId : fileDataStruct_.abstractCommonResourcesStruct->resourceHostRegionIDs.value()) {
                appendArrayElement(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs, regionId);
            }
        }
    }
    if (fileDataStruct_.abstractDatasetStruct.has_value()) {
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::Name, fileDataStruct_.abstractDatasetStruct.value().name);
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::Description, fileDataStruct_.abstractDatasetStruct.value().description);
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::TotalSize, fileDataStruct_.abstractDatasetStruct.value().totalSize);
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::EncodingFormatTypeID, fileDataStruct_.abstractDatasetStruct.value().encodingFormatTypeID);
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::SchemaFormatTypeID, fileDataStruct_.abstractDatasetStruct.value().schemaFormatTypeID);
        setOptionalParameter(OsduGenericFileCollectionDataLiterals::Endian, fileDataStruct_.abstractDatasetStruct.value().endian);
    }
    if (fileDataStruct_.abstractFileCollectionStruct.has_value()) {

        const auto abstractFile = OsduAbstractFileCollection(fileDataStruct_.abstractFileCollectionStruct.value());
        setParameter(OsduGenericFileCollectionDataLiterals::DatasetProperties, abstractFile.getAsJson());
    }

    if (fileDataStruct_.extensionPropertiesArrayStruct.has_value()) {
        const auto extensionProperties = OsduExtensionPropertyArray(fileDataStruct_.extensionPropertiesArrayStruct.value());
        setParameter(OsduGenericFileCollectionDataLiterals::ExtensionProperties, extensionProperties.getAsJson());
    }
}

void
OsduGenericFileCollectionData::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.

    fileDataStruct_ = OsduGenericFileCollectionDataStruct(getOsduPartition());
}

void
OsduGenericFileCollectionData::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduAbstractCommonResourcesLiterals::ExistenceKind) || isMember(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID) || isMember(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs) || isMember(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus) || isMember(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus) || isMember(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification)) {
        fileDataStruct_.abstractCommonResourcesStruct = getAbstractCommonResourcesStruct();
    }
    if (isMember(OsduGenericFileCollectionDataLiterals::DatasetProperties) || isMember(OsduGenericFileCollectionDataLiterals::Checksum)) {
        fileDataStruct_.abstractFileCollectionStruct = getAbstractFileCollectionStruct();
    }
    if (isMember(OsduGenericFileCollectionDataLiterals::Name) || isMember(OsduGenericFileCollectionDataLiterals::Description) || isMember(OsduGenericFileCollectionDataLiterals::EncodingFormatTypeID) || isMember(OsduGenericFileCollectionDataLiterals::SchemaFormatTypeID)) {
        fileDataStruct_.abstractDatasetStruct = getAbstractDatasetStruct();
    }

    if (isMember(OsduGenericFileCollectionDataLiterals::ExtensionProperties)) {
        fileDataStruct_.extensionPropertiesArrayStruct = getExtensionPropertiesStruct();
    }
}

std::optional<OsduAbstractCommonResourcesStruct>
OsduGenericFileCollectionData::getAbstractCommonResourcesStruct() const
{
    OsduAbstractCommonResources abstractCommonResources(getAsJson(), getOsduPartition());
    const auto commonResourcesStructPtr = std::reinterpret_pointer_cast<OsduAbstractCommonResourcesStruct>(abstractCommonResources.getMetaDataStruct());
    if (commonResourcesStructPtr != nullptr) {

        return *commonResourcesStructPtr;
    }

    return std::nullopt;
}

std::optional<OsduAbstractDatasetStruct>
OsduGenericFileCollectionData::getAbstractDatasetStruct() const
{
    OsduAbstractDataset abstractDataset(getAsJson(), getOsduPartition());
    const auto datasetStructPtr = std::reinterpret_pointer_cast<OsduAbstractDatasetStruct>(abstractDataset.getMetaDataStruct());
    if (datasetStructPtr != nullptr) {

        return *datasetStructPtr;
    }

    return std::nullopt;
}

std::optional<OsduAbstractFileCollectionStruct>
OsduGenericFileCollectionData::getAbstractFileCollectionStruct() const
{
    if (isMember(OsduAbstractFileCollectionLiterals::DatasetProperties)) {
        OsduAbstractFileCollection abstractFileCollection(getParameter<Json::Value>(OsduAbstractFileCollectionLiterals::DatasetProperties), getOsduPartition());
        const auto fileCollectionStructPtr = std::reinterpret_pointer_cast<OsduAbstractFileCollectionStruct>(abstractFileCollection.getMetaDataStruct());
        if (fileCollectionStructPtr != nullptr) {

            return *fileCollectionStructPtr;
        }
    }

    return std::nullopt;
}

std::optional<OsduExtensionPropertyArrayStruct>
OsduGenericFileCollectionData::getExtensionPropertiesStruct() const
{
    if (!isMember(OsduGenericFileCollectionDataLiterals::ExtensionProperties)) {
        return std::nullopt;
    }

    OsduExtensionPropertyArray extensionPropertyArray(getParameter<Json::Value>(OsduGenericFileCollectionDataLiterals::ExtensionProperties), getOsduPartition());
    const auto extensionPropertiesArrayStruct = std::reinterpret_pointer_cast<OsduExtensionPropertyArrayStruct>(extensionPropertyArray.getMetaDataStruct());
    if (extensionPropertiesArrayStruct != nullptr) {

        return *extensionPropertiesArrayStruct;
    }

    return std::nullopt;
}

std::shared_ptr<OsduMetadataStructBase>
OsduGenericFileCollectionData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGenericFileCollectionDataStruct>(fileDataStruct_);
}

} // namespace segysdk::metadata::osdu
