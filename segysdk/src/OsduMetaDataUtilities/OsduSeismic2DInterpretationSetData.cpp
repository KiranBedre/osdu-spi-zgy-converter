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

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractMaster.h"
#include "../OsduMetaDataUtilities/OsduAbstractProject.h"
#include "../OsduMetaDataUtilities/OsduAbstractProjectActivity.h"
#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryRef.h"

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismic2DInterpretationSetData::OsduSeismic2DInterpretationSetData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismic2DInterpretationSetDataStruct_(osduPartition)
{
    /* Empty */
}
OsduSeismic2DInterpretationSetData::OsduSeismic2DInterpretationSetData(OsduSeismic2DInterpretationSetDataStruct seismic2DInterpretationSetDataStruct)
  : OsduMetadataBase(seismic2DInterpretationSetDataStruct.getPartition()), seismic2DInterpretationSetDataStruct_(std::move(seismic2DInterpretationSetDataStruct))
{
    populateJsonObject();
}

OsduSeismic2DInterpretationSetData::OsduSeismic2DInterpretationSetData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismic2DInterpretationSetDataStruct_(OsduSeismic2DInterpretationSetDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismic2DInterpretationSetData::isValidInput() const
{
    return seismic2DInterpretationSetDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismic2DInterpretationSetData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismic2DInterpretationSetDataStruct>(seismic2DInterpretationSetDataStruct_);
}

void
OsduSeismic2DInterpretationSetData::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismic2DInterpretationSetData::populateJsonObject()
{
    initializeJson();

    if (seismic2DInterpretationSetDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJson(OsduAbstractCommonResources(seismic2DInterpretationSetDataStruct_.commonResourcesStruct.value()).getAsJson());
    }
    if (seismic2DInterpretationSetDataStruct_.masterStruct.has_value()) {
        mergeInJson(OsduAbstractMaster(seismic2DInterpretationSetDataStruct_.masterStruct.value()).getAsJson());
    }
    if (seismic2DInterpretationSetDataStruct_.projectStruct.has_value()) {
        mergeInJson(OsduAbstractProject(seismic2DInterpretationSetDataStruct_.projectStruct.value()).getAsJson());
    }
    if (seismic2DInterpretationSetDataStruct_.activityStruct.has_value()) {
        mergeInJson(OsduAbstractProjectActivity(seismic2DInterpretationSetDataStruct_.activityStruct.value()).getAsJson());
    }
    setOptionalArrayStructParameter<OsduSeismicLineGeometryRef, OsduSeismicLineGeometryRefStruct>(OsduSeismic2DInterpretationSetDataLiterals::SeismicLineGeometries, seismic2DInterpretationSetDataStruct_.seismicLineGeometriesStruct);
}

void
OsduSeismic2DInterpretationSetData::initializeMetadataStruct()
{
    seismic2DInterpretationSetDataStruct_ = OsduSeismic2DInterpretationSetDataStruct(getOsduPartition());
}

void
OsduSeismic2DInterpretationSetData::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismic2DInterpretationSetDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    seismic2DInterpretationSetDataStruct_.masterStruct = getOptionalStructParameter<OsduAbstractMaster, OsduAbstractMasterStruct>();
    seismic2DInterpretationSetDataStruct_.projectStruct = getOptionalStructParameter<OsduAbstractProject, OsduAbstractProjectStruct>();
    seismic2DInterpretationSetDataStruct_.activityStruct = getOptionalStructParameter<OsduAbstractProjectActivity, OsduAbstractProjectActivityStruct>();
    seismic2DInterpretationSetDataStruct_.seismicLineGeometriesStruct = getOptionalArrayStructParameter<OsduSeismicLineGeometryRef, OsduSeismicLineGeometryRefStruct>(OsduSeismic2DInterpretationSetDataLiterals::SeismicLineGeometries);
}
} // namespace segysdk::metadata::osdu
