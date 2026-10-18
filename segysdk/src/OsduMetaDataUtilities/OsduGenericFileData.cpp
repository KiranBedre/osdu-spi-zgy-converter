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

#include "../OsduMetaDataUtilities/OsduGenericFileData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesLiterals.h"
#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"
#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractFile.h"
#include "../OsduMetaDataUtilities/OsduGenericFileDataStruct.h"
#include "../OsduMetaDataUtilities/OsduGenericFileDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGenericFileData::OsduGenericFileData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileDataStruct_(OsduGenericFileDataStruct(osduPartition)) {}

OsduGenericFileData::OsduGenericFileData(const OsduGenericFileData& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    fileDataStruct_(other.fileDataStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileData::OsduGenericFileData(OsduGenericFileData&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    fileDataStruct_(std::move(other.fileDataStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileData&
OsduGenericFileData::operator=(const OsduGenericFileData& rhs)
{
    return *this = OsduGenericFileData(rhs);
}

OsduGenericFileData&
OsduGenericFileData::operator=(OsduGenericFileData&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    fileDataStruct_ = std::move(rhs.fileDataStruct_);

    return *this;
}

OsduGenericFileData::OsduGenericFileData(OsduGenericFileDataStruct fileDataStruct)
  : OsduMetadataBase(fileDataStruct.getPartition(), false), fileDataStruct_(std::move(fileDataStruct))
{
    populateJsonObject();
}

OsduGenericFileData::OsduGenericFileData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileDataStruct_(OsduGenericFileDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGenericFileData::isValidInput() const
{
    return fileDataStruct_.isValid();
}

void
OsduGenericFileData::initializeJson()
{
    removeMember(OsduGenericFileDataLiterals::ContentSchemaKind);
    removeMember(OsduGenericFileDataLiterals::ExtensionProperties);
    removeMember(OsduGenericFileDataLiterals::Data);
    removeMember(OsduGenericFileDataLiterals::Name);
    removeMember(OsduGenericFileDataLiterals::Description);
    removeMember(OsduGenericFileDataLiterals::TotalSize);
    removeMember(OsduGenericFileDataLiterals::EncodingFormatTypeID);
    removeMember(OsduGenericFileDataLiterals::SchemaFormatTypeID);
    removeMember(OsduGenericFileDataLiterals::Endian);
    removeMember(OsduGenericFileDataLiterals::DatasetProperties);
    removeMember(OsduAbstractCommonResourcesLiterals::ExistenceKind);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::Source);
}

void
OsduGenericFileData::populateJsonObject()
{
    clearJsonObject();

    if (fileDataStruct_.abstractCommonResourcesStruct.has_value()) {
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ExistenceKind, fileDataStruct_.abstractCommonResourcesStruct.value().existenceKind);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus, fileDataStruct_.abstractCommonResourcesStruct.value().resourceCurationStatus);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification, fileDataStruct_.abstractCommonResourcesStruct.value().resourceSecurityClassification);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus, fileDataStruct_.abstractCommonResourcesStruct.value().resourceLifecycleStatus);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::Source, fileDataStruct_.abstractCommonResourcesStruct.value().source);
        setOptionalParameter(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID, fileDataStruct_.abstractCommonResourcesStruct.value().resourceHomeRegionID);
        if (fileDataStruct_.abstractCommonResourcesStruct.value().resourceHostRegionIDs.has_value()) {
            InitializeJsonArray(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs);
            for (const auto& regionId : fileDataStruct_.abstractCommonResourcesStruct.value().resourceHostRegionIDs.value()) {
                appendArrayElement(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs, regionId);
            }
        }
    }
    if (fileDataStruct_.abstractDatasetStruct.has_value()) {
        setOptionalParameter(OsduGenericFileDataLiterals::Name, fileDataStruct_.abstractDatasetStruct.value().name);
        setOptionalParameter(OsduGenericFileDataLiterals::Description, fileDataStruct_.abstractDatasetStruct.value().description);
        setOptionalParameter(OsduGenericFileDataLiterals::TotalSize, fileDataStruct_.abstractDatasetStruct.value().totalSize);
        setOptionalParameter(OsduGenericFileDataLiterals::EncodingFormatTypeID, fileDataStruct_.abstractDatasetStruct.value().encodingFormatTypeID);
        setOptionalParameter(OsduGenericFileDataLiterals::SchemaFormatTypeID, fileDataStruct_.abstractDatasetStruct.value().schemaFormatTypeID);
        setOptionalParameter(OsduGenericFileDataLiterals::Endian, fileDataStruct_.abstractDatasetStruct.value().endian);
    }

    if (fileDataStruct_.abstractFileStruct.has_value()) {

        const auto abstractFile = OsduAbstractFile(fileDataStruct_.abstractFileStruct.value());
        setParameter(OsduGenericFileDataLiterals::DatasetProperties, abstractFile.getAsJson());

        setOptionalParameter(OsduGenericFileDataLiterals::Checksum, fileDataStruct_.abstractFileStruct.value().checksum);
    }

    if (fileDataStruct_.extensionPropertiesArrayStruct.has_value()) {

        const auto extensionProperties = OsduExtensionPropertyArray(fileDataStruct_.extensionPropertiesArrayStruct.value());
        setParameter(OsduGenericFileDataLiterals::ExtensionProperties, extensionProperties.getAsJson());
    }
}

void
OsduGenericFileData::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.

    fileDataStruct_ = OsduGenericFileDataStruct(getOsduPartition());
}

void
OsduGenericFileData::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduAbstractCommonResourcesLiterals::ExistenceKind) or isMember(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID) or isMember(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs) or isMember(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus) or isMember(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus) or isMember(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification)) {
        fileDataStruct_.abstractCommonResourcesStruct = getAbstractCommonResourcesStruct();
    }
    if (isMember(OsduGenericFileDataLiterals::DatasetProperties) or isMember(OsduGenericFileDataLiterals::Checksum)) {
        fileDataStruct_.abstractFileStruct = getAbstractFileStruct();
    }
    if (isMember(OsduGenericFileDataLiterals::Name) or isMember(OsduGenericFileDataLiterals::Description) or isMember(OsduGenericFileDataLiterals::EncodingFormatTypeID) or isMember(OsduGenericFileDataLiterals::SchemaFormatTypeID)) {
        fileDataStruct_.abstractDatasetStruct = getAbstractDatasetStruct();
    }

    if (isMember(OsduGenericFileDataLiterals::ExtensionProperties)) {
        fileDataStruct_.extensionPropertiesArrayStruct = getExtensionPropertiesStruct();
    }
}

std::optional<OsduAbstractCommonResourcesStruct>
OsduGenericFileData::getAbstractCommonResourcesStruct() const
{
    OsduAbstractCommonResources abstractCommonResources(getAsJson(), getOsduPartition());
    const auto commonResourcesStructPtr = std::reinterpret_pointer_cast<OsduAbstractCommonResourcesStruct>(abstractCommonResources.getMetaDataStruct());
    if (commonResourcesStructPtr != nullptr) {

        return *commonResourcesStructPtr;
    }

    return std::nullopt;
}

std::optional<OsduAbstractDatasetStruct>
OsduGenericFileData::getAbstractDatasetStruct() const
{
    OsduAbstractDataset abstractDataset(getAsJson(), getOsduPartition());
    const auto datasetStructPtr = std::reinterpret_pointer_cast<OsduAbstractDatasetStruct>(abstractDataset.getMetaDataStruct());
    if (datasetStructPtr != nullptr) {

        return *datasetStructPtr;
    }

    return std::nullopt;
}

std::optional<OsduAbstractFileStruct>
OsduGenericFileData::getAbstractFileStruct() const
{
    OsduAbstractFile abstractFile(getAsJson(), getOsduPartition());
    const auto fileStructPtr = std::reinterpret_pointer_cast<OsduAbstractFileStruct>(abstractFile.getMetaDataStruct());
    if (fileStructPtr != nullptr) {

        return *fileStructPtr;
    }

    return std::nullopt;
}

std::optional<OsduExtensionPropertyArrayStruct>
OsduGenericFileData::getExtensionPropertiesStruct() const
{
    if (!isMember(OsduGenericFileDataLiterals::ExtensionProperties)) {
        return std::nullopt;
    }

    OsduExtensionPropertyArray extensionPropertyArray(getParameter<Json::Value>(OsduGenericFileDataLiterals::ExtensionProperties), getOsduPartition());
    const auto extensionPropertiesArrayStruct = std::reinterpret_pointer_cast<OsduExtensionPropertyArrayStruct>(extensionPropertyArray.getMetaDataStruct());
    if (extensionPropertiesArrayStruct != nullptr) {

        return *extensionPropertiesArrayStruct;
    }

    return std::nullopt;
}

std::shared_ptr<OsduMetadataStructBase>
OsduGenericFileData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGenericFileDataStruct>(fileDataStruct_);
}

} // namespace segysdk::metadata::osdu
