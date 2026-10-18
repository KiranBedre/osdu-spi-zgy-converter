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

#include "../OsduMetaDataUtilities/OsduFileCollectionSegyData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileCollection.h"
#include "../OsduMetaDataUtilities/OsduVectorHeaderMapping.h"
#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeader.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeader.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecision.h"

#include "../OsduMetaDataUtilities/OsduFileCollectionSegyDataLiterals.h"

namespace segysdk::metadata::osdu
{

OsduFileCollectionSegyData::OsduFileCollectionSegyData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), segyDataStruct_(OsduFileCollectionSegyDataStruct(osduPartition))
{
    /* Empty */
}

OsduFileCollectionSegyData::OsduFileCollectionSegyData(OsduFileCollectionSegyDataStruct segyDataStruct)
    : OsduMetadataBase(segyDataStruct.getPartition()), segyDataStruct_(std::move(segyDataStruct))
{
    populateJsonObject();
}

OsduFileCollectionSegyData::OsduFileCollectionSegyData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), segyDataStruct_(OsduFileCollectionSegyDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileCollectionSegyData::isValidInput() const
{
    return segyDataStruct_.isValid();
}

void
OsduFileCollectionSegyData::initializeJson()
{
    clearJsonObject();
}

void
OsduFileCollectionSegyData::populateJsonObject()
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

    setOptionalStructParameter<OsduVectorHeaderMapping>(OsduFileCollectionSegyDataLiterals::VectorHeaderMapping, segyDataStruct_.vectorHeaderMappingStruct);
    setOptionalStructParameter<OsduOverridesBinaryHeader>(OsduFileCollectionSegyDataLiterals::BinaryHeaderOverrides, segyDataStruct_.binaryHeaderOverridesStruct);
    setOptionalStructParameter<OsduOverridesTraceHeader>(OsduFileCollectionSegyDataLiterals::TraceHeaderOverrides, segyDataStruct_.traceHeaderOverridesStruct);
    
    setOptionalParameter(OsduFileCollectionSegyDataLiterals::SegyRevision, segyDataStruct_.segyRevision);

    setOptionalStructParameter<OsduAbstractSeismicPrecision>(OsduFileCollectionSegyDataLiterals::Precision, segyDataStruct_.precisionStruct);

    setOptionalParameter(OsduFileCollectionSegyDataLiterals::Is3D, segyDataStruct_.is3D);
    setOptionalParameter(OsduFileCollectionSegyDataLiterals::StorageFacilityID, segyDataStruct_.storageFacilityID);

    setOptionalArrayParameter(OsduFileCollectionSegyDataLiterals::TapeLabels, segyDataStruct_.tapeLabels);
    setOptionalArrayParameter(OsduFileCollectionSegyDataLiterals::BoxLabels, segyDataStruct_.boxLabels);
}

void
OsduFileCollectionSegyData::initializeMetadataStruct()
{
    segyDataStruct_ = OsduFileCollectionSegyDataStruct(getOsduPartition());
}

void
OsduFileCollectionSegyData::populateMetadataStruct()
{
    initializeMetadataStruct();

    segyDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    segyDataStruct_.datasetStruct = getOptionalStructParameter<OsduAbstractDataset, OsduAbstractDatasetStruct>();
    segyDataStruct_.fileCollectionStruct = getOptionalStructParameter<OsduAbstractFileCollection, OsduAbstractFileCollectionStruct>();
    segyDataStruct_.vectorHeaderMappingStruct = getOptionalStructParameter<OsduVectorHeaderMapping, OsduVectorHeaderMappingStruct>(OsduFileCollectionSegyDataLiterals::VectorHeaderMapping);
    segyDataStruct_.binaryHeaderOverridesStruct = getOptionalStructParameter<OsduOverridesBinaryHeader, OsduOverridesBinaryHeaderStruct>(OsduFileCollectionSegyDataLiterals::BinaryHeaderOverrides);
    segyDataStruct_.traceHeaderOverridesStruct = getOptionalStructParameter<OsduOverridesTraceHeader, OsduOverridesTraceHeaderStruct>(OsduFileCollectionSegyDataLiterals::TraceHeaderOverrides);

    segyDataStruct_.segyRevision = getOptionalParameter<std::string>(OsduFileCollectionSegyDataLiterals::SegyRevision);

    segyDataStruct_.precisionStruct = getOptionalStructParameter<OsduAbstractSeismicPrecision, OsduAbstractSeismicPrecisionStruct>(OsduFileCollectionSegyDataLiterals::Precision);

    segyDataStruct_.is3D = getOptionalParameter<bool>(OsduFileCollectionSegyDataLiterals::Is3D);
    segyDataStruct_.storageFacilityID = getOptionalParameter<std::string>(OsduFileCollectionSegyDataLiterals::StorageFacilityID);

    segyDataStruct_.tapeLabels = getOptionalArrayParameter<std::string>(OsduFileCollectionSegyDataLiterals::TapeLabels);
    segyDataStruct_.boxLabels = getOptionalArrayParameter<std::string>(OsduFileCollectionSegyDataLiterals::BoxLabels);
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileCollectionSegyData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileCollectionSegyDataStruct>(segyDataStruct_);
}
} // namespace segysdk::metadata::osdu
