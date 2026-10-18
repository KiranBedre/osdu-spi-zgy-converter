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

#include "../OsduMetaDataUtilities/OsduSeismicBinGridData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupType.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponent.h"
#include "../OsduMetaDataUtilities/OsduAbstractBinGrid.h"

namespace segysdk::metadata::osdu
{
OsduSeismicBinGridData::OsduSeismicBinGridData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicBinGridDataStruct_(OsduSeismicBinGridDataStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicBinGridData::OsduSeismicBinGridData(OsduSeismicBinGridDataStruct seismicBinGridDataStruct)
  : OsduMetadataBase(seismicBinGridDataStruct.getPartition()), seismicBinGridDataStruct_(std::move(seismicBinGridDataStruct))
{
    populateJsonObject();
}

OsduSeismicBinGridData::OsduSeismicBinGridData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicBinGridDataStruct_(OsduSeismicBinGridDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicBinGridData::isValidInput() const
{
    return seismicBinGridDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicBinGridData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicBinGridDataStruct>(seismicBinGridDataStruct_);
}

void
OsduSeismicBinGridData::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicBinGridData::populateJsonObject()
{
    initializeJson();

    if (seismicBinGridDataStruct_.commonResourcesStruct.has_value()) {
        mergeInJson(OsduAbstractCommonResources(seismicBinGridDataStruct_.commonResourcesStruct.value()).getAsJson());
    }
    if (seismicBinGridDataStruct_.wpcGroupTypeStruct.has_value()) {
        mergeInJson(OsduAbstractWPCGroupType(seismicBinGridDataStruct_.wpcGroupTypeStruct.value()).getAsJson());
    }
    if (seismicBinGridDataStruct_.workProductComponentStruct.has_value()) {
        mergeInJson(OsduAbstractWorkProductComponent(seismicBinGridDataStruct_.workProductComponentStruct.value()).getAsJson());
    }
    if (seismicBinGridDataStruct_.binGridStruct.has_value()) {
        mergeInJson(OsduAbstractBinGrid(seismicBinGridDataStruct_.binGridStruct.value()).getAsJson());
    }
}

void
OsduSeismicBinGridData::initializeMetadataStruct()
{
    seismicBinGridDataStruct_ = OsduSeismicBinGridDataStruct(getOsduPartition());
}

void
OsduSeismicBinGridData::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicBinGridDataStruct_.commonResourcesStruct = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    seismicBinGridDataStruct_.wpcGroupTypeStruct = getOptionalStructParameter<OsduAbstractWPCGroupType, OsduAbstractWPCGroupTypeStruct>();
    seismicBinGridDataStruct_.workProductComponentStruct = getOptionalStructParameter<OsduAbstractWorkProductComponent, OsduAbstractWorkProductComponentStruct>();
    seismicBinGridDataStruct_.binGridStruct = getOptionalStructParameter<OsduAbstractBinGrid, OsduAbstractBinGridStruct>();
}
} // namespace segysdk::metadata::osdu
