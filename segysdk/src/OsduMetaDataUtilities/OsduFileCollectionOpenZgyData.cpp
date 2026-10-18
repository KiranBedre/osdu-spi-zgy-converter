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

#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractCompressionInfo.h"

#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileCollection.h"
#include "../OsduMetaDataUtilities/OsduVectorHeaderMapping.h"
#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeader.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeader.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecision.h"

#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyDataLiterals.h"

namespace segysdk::metadata::osdu
{

OsduFileCollectionOpenZgyData::OsduFileCollectionOpenZgyData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), segyDataStruct_(OsduFileCollectionOpenZgyDataStruct(osduPartition))
{
    /* Empty */
}

OsduFileCollectionOpenZgyData::OsduFileCollectionOpenZgyData(OsduFileCollectionOpenZgyDataStruct segyDataStruct)
    : OsduMetadataBase(segyDataStruct.getPartition()), segyDataStruct_(std::move(segyDataStruct))
{
    populateJsonObject();
}

OsduFileCollectionOpenZgyData::OsduFileCollectionOpenZgyData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), segyDataStruct_(OsduFileCollectionOpenZgyDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileCollectionOpenZgyData::isValidInput() const
{
    return segyDataStruct_.isValid();
}

void
OsduFileCollectionOpenZgyData::initializeJson()
{
    clearJsonObject();
}

void
OsduFileCollectionOpenZgyData::populateJsonObject()
{
    initializeJson();

    if (segyDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractCommonResources(segyDataStruct_.commonResourcesStruct.value()).getAsJson());
    }
    if (segyDataStruct_.datasetStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractDataset(segyDataStruct_.datasetStruct.value()).getAsJson());
    }
    if (segyDataStruct_.fileCollectionStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractFileCollection(segyDataStruct_.fileCollectionStruct.value()).getAsJson());
    }

    setOptionalStructParameter<OsduAbstractCompressionInfo>(OsduFileCollectionOpenZgyDataLiterals::Compression, segyDataStruct_.compressionInfoStruct);
    setOptionalStructParameter<OsduAbstractSeismicPrecision>(OsduFileCollectionOpenZgyDataLiterals::Precision, segyDataStruct_.precisionStruct);


}

void
OsduFileCollectionOpenZgyData::initializeMetadataStruct()
{
    segyDataStruct_ = OsduFileCollectionOpenZgyDataStruct(getOsduPartition());
}

void
OsduFileCollectionOpenZgyData::populateMetadataStruct()
{
    initializeMetadataStruct();

    segyDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    segyDataStruct_.datasetStruct = getOptionalStructParameter<OsduAbstractDataset, OsduAbstractDatasetStruct>();
    segyDataStruct_.fileCollectionStruct = getOptionalStructParameter<OsduAbstractFileCollection, OsduAbstractFileCollectionStruct>();
    segyDataStruct_.compressionInfoStruct = getOptionalStructParameter<OsduAbstractCompressionInfo, OsduAbstractCompressionInfoStruct>(OsduFileCollectionOpenZgyDataLiterals::Compression);

    segyDataStruct_.precisionStruct = getOptionalStructParameter<OsduAbstractSeismicPrecision, OsduAbstractSeismicPrecisionStruct>(OsduFileCollectionOpenZgyDataLiterals::Precision);

}

std::shared_ptr<OsduMetadataStructBase>
OsduFileCollectionOpenZgyData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileCollectionOpenZgyDataStruct>(segyDataStruct_);
}
} // namespace segysdk::metadata::osdu
