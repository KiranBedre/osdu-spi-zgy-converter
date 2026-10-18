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

#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractFile.h"

#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileSeismicLineGeometryData::OsduFileSeismicLineGeometryData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileSeismicLineGeometryDataStruct_(OsduFileSeismicLineGeometryDataStruct(osduPartition))
{
    /* Empty */
}

OsduFileSeismicLineGeometryData::OsduFileSeismicLineGeometryData(OsduFileSeismicLineGeometryDataStruct fileSeismicLineGeometryStruct)
  : OsduMetadataBase(fileSeismicLineGeometryStruct.getPartition()), fileSeismicLineGeometryDataStruct_(std::move(fileSeismicLineGeometryStruct))
{
    populateJsonObject();
}

OsduFileSeismicLineGeometryData::OsduFileSeismicLineGeometryData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileSeismicLineGeometryDataStruct_(OsduFileSeismicLineGeometryDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileSeismicLineGeometryData::isValidInput() const
{
    return fileSeismicLineGeometryDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileSeismicLineGeometryData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileSeismicLineGeometryDataStruct>(fileSeismicLineGeometryDataStruct_);
}

void
OsduFileSeismicLineGeometryData::initializeJson()
{
    clearJsonObject();
}

void
OsduFileSeismicLineGeometryData::populateJsonObject()
{
    initializeJson();

    if (fileSeismicLineGeometryDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJson(OsduAbstractCommonResources(fileSeismicLineGeometryDataStruct_.commonResourcesStruct.value()).getAsJson());
    }

    if (fileSeismicLineGeometryDataStruct_.datasetStruct.has_value()) {
        mergeInJson(OsduAbstractDataset(fileSeismicLineGeometryDataStruct_.datasetStruct.value()).getAsJson());
    }

    if (fileSeismicLineGeometryDataStruct_.fileStruct.has_value()) {
        mergeInJson(OsduAbstractFile(fileSeismicLineGeometryDataStruct_.fileStruct.value()).getAsJson());
    }
    
    setOptionalParameter(OsduFileSeismicLineGeometryDataLiterals::ContentSchemaKind, fileSeismicLineGeometryDataStruct_.contentSchemaKind);
}

void
OsduFileSeismicLineGeometryData::initializeMetadataStruct()
{
    fileSeismicLineGeometryDataStruct_ = OsduFileSeismicLineGeometryDataStruct(getOsduPartition());
}

void
OsduFileSeismicLineGeometryData::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileSeismicLineGeometryDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    fileSeismicLineGeometryDataStruct_.datasetStruct = getOptionalStructParameter<OsduAbstractDataset, OsduAbstractDatasetStruct>();
    fileSeismicLineGeometryDataStruct_.fileStruct = getOptionalStructParameter<OsduAbstractFile, OsduAbstractFileStruct>();
    fileSeismicLineGeometryDataStruct_.contentSchemaKind = getOptionalParameter<std::string>(OsduFileSeismicLineGeometryDataLiterals::ContentSchemaKind);
}
} // namespace segysdk::metadata::osdu
