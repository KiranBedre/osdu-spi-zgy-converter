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

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupType.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponent.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicLineGeometryData::OsduSeismicLineGeometryData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicLineGeometryDataStruct_(OsduSeismicLineGeometryDataStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicLineGeometryData::OsduSeismicLineGeometryData(OsduSeismicLineGeometryDataStruct seismicLineGeometryStruct)
  : OsduMetadataBase(seismicLineGeometryStruct.getPartition()), seismicLineGeometryDataStruct_(std::move(seismicLineGeometryStruct))
{
    populateJsonObject();
}

OsduSeismicLineGeometryData::OsduSeismicLineGeometryData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicLineGeometryDataStruct_(OsduSeismicLineGeometryDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicLineGeometryData::isValidInput() const
{
    return seismicLineGeometryDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicLineGeometryData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicLineGeometryDataStruct>(seismicLineGeometryDataStruct_);
}

void
OsduSeismicLineGeometryData::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicLineGeometryData::populateJsonObject()
{
    initializeJson();

    if (seismicLineGeometryDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJson(OsduAbstractCommonResources(seismicLineGeometryDataStruct_.commonResourcesStruct.value()).getAsJson());
    }
    if (seismicLineGeometryDataStruct_.wpcGroupTypeStruct.has_value()) {
        mergeInJson(OsduAbstractWPCGroupType(seismicLineGeometryDataStruct_.wpcGroupTypeStruct.value()).getAsJson());
    }
    if (seismicLineGeometryDataStruct_.workProductComponentStruct.has_value()) {
        mergeInJson(OsduAbstractWorkProductComponent(seismicLineGeometryDataStruct_.workProductComponentStruct.value()).getAsJson());
    }

    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::NotionalSeismicLineID, seismicLineGeometryDataStruct_.notionalSeismicLineID);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::Preferred2DInterpretationSetID, seismicLineGeometryDataStruct_.preferred2DInterpretationSetID);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::HorizontalCRSID, seismicLineGeometryDataStruct_.horizontalCRSID);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::FirstCMP, seismicLineGeometryDataStruct_.firstCMP);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::LastCMP, seismicLineGeometryDataStruct_.lastCMP);

    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduSeismicLineGeometryDataLiterals::FirstLocation, seismicLineGeometryDataStruct_.firstLocation);
    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduSeismicLineGeometryDataLiterals::LastLocation, seismicLineGeometryDataStruct_.lastLocation);

    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::FirstStationLabel, seismicLineGeometryDataStruct_.firstStationLabel);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::LastStationLabel, seismicLineGeometryDataStruct_.lastStationLabel);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::HasMonotonicLabelling, seismicLineGeometryDataStruct_.hasMonotonicLabelling);
    setOptionalParameter(OsduSeismicLineGeometryDataLiterals::HasCMPIncreaseByOne, seismicLineGeometryDataStruct_.hasCMPIncreaseByOne);
}

void
OsduSeismicLineGeometryData::initializeMetadataStruct()
{
    seismicLineGeometryDataStruct_ = OsduSeismicLineGeometryDataStruct(getOsduPartition());
}

void
OsduSeismicLineGeometryData::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicLineGeometryDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    seismicLineGeometryDataStruct_.wpcGroupTypeStruct = getOptionalStructParameter<OsduAbstractWPCGroupType, OsduAbstractWPCGroupTypeStruct>();
    seismicLineGeometryDataStruct_.workProductComponentStruct = getOptionalStructParameter<OsduAbstractWorkProductComponent, OsduAbstractWorkProductComponentStruct>();

    seismicLineGeometryDataStruct_.notionalSeismicLineID = getOptionalParameter<std::string>(OsduSeismicLineGeometryDataLiterals::NotionalSeismicLineID);
    seismicLineGeometryDataStruct_.preferred2DInterpretationSetID = getOptionalParameter<std::string>(OsduSeismicLineGeometryDataLiterals::Preferred2DInterpretationSetID);
    seismicLineGeometryDataStruct_.horizontalCRSID = getOptionalParameter<std::string>(OsduSeismicLineGeometryDataLiterals::HorizontalCRSID);
    seismicLineGeometryDataStruct_.firstCMP = getOptionalParameter<double>(OsduSeismicLineGeometryDataLiterals::FirstCMP);
    seismicLineGeometryDataStruct_.lastCMP = getOptionalParameter<double>(OsduSeismicLineGeometryDataLiterals::LastCMP);

    seismicLineGeometryDataStruct_.firstLocation = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduSeismicLineGeometryDataLiterals::FirstLocation);
    seismicLineGeometryDataStruct_.lastLocation = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduSeismicLineGeometryDataLiterals::LastLocation);

    seismicLineGeometryDataStruct_.firstStationLabel = getOptionalParameter<std::string>(OsduSeismicLineGeometryDataLiterals::FirstStationLabel);
    seismicLineGeometryDataStruct_.lastStationLabel = getOptionalParameter<std::string>(OsduSeismicLineGeometryDataLiterals::LastStationLabel);
    seismicLineGeometryDataStruct_.hasMonotonicLabelling = getOptionalParameter<bool>(OsduSeismicLineGeometryDataLiterals::HasMonotonicLabelling);
    seismicLineGeometryDataStruct_.hasCMPIncreaseByOne = getOptionalParameter<bool>(OsduSeismicLineGeometryDataLiterals::HasCMPIncreaseByOne);
}
} // namespace segysdk::metadata::osdu
