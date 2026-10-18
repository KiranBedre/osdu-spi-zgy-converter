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

#include "../OsduMetaDataUtilities/OsduFileHistogramData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractFile.h"

#include "../OsduMetaDataUtilities/OsduFileHistogramDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileHistogramData::OsduFileHistogramData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileHistogramDataStruct_(OsduFileHistogramDataStruct(osduPartition)) {}

OsduFileHistogramData::OsduFileHistogramData(OsduFileHistogramDataStruct fileHistogramDataStruct)
  : OsduMetadataBase(fileHistogramDataStruct.getPartition(), false), fileHistogramDataStruct_(std::move(fileHistogramDataStruct))
{
    populateJsonObject();
}

OsduFileHistogramData::OsduFileHistogramData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileHistogramDataStruct_(OsduFileHistogramDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileHistogramData::isValidInput() const
{
    return fileHistogramDataStruct_.isValid();
}

void
OsduFileHistogramData::initializeJson()
{
    clearJsonObject();
}

void
OsduFileHistogramData::populateJsonObject()
{
    initializeJson();

    if (fileHistogramDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractCommonResources(fileHistogramDataStruct_.commonResourcesStruct.value()).getAsJson());
    }
    if (fileHistogramDataStruct_.datasetStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractDataset(fileHistogramDataStruct_.datasetStruct.value()).getAsJson());
    }
    if (fileHistogramDataStruct_.fileStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractFile(fileHistogramDataStruct_.fileStruct.value()).getAsJson());
    }

    setOptionalParameter(OsduFileHistogramDataLiterals::ContentSchemaKind, fileHistogramDataStruct_.contentSchemaKind);
}

void
OsduFileHistogramData::initializeMetadataStruct()
{
    fileHistogramDataStruct_ = OsduFileHistogramDataStruct(getOsduPartition());
}

void
OsduFileHistogramData::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileHistogramDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    fileHistogramDataStruct_.datasetStruct = getOptionalStructParameter<OsduAbstractDataset, OsduAbstractDatasetStruct>();
    fileHistogramDataStruct_.fileStruct = getOptionalStructParameter<OsduAbstractFile, OsduAbstractFileStruct>();

    fileHistogramDataStruct_.contentSchemaKind = getOptionalParameter<std::string>(OsduFileHistogramDataLiterals::ContentSchemaKind);
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileHistogramData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileHistogramDataStruct>(fileHistogramDataStruct_);
}
} // namespace segysdk::metadata::osdu
