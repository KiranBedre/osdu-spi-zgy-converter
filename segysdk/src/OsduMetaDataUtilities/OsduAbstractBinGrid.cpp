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

#include "../OsduMetaDataUtilities/OsduAbstractBinGrid.h"

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"

#include "../OsduMetaDataUtilities/OsduAbstractBinGridLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractBinGrid::OsduAbstractBinGrid(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    binGridStruct_(OsduAbstractBinGridStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractBinGrid::OsduAbstractBinGrid(OsduAbstractBinGridStruct binGridStruct)
  : OsduMetadataBase(binGridStruct.getPartition()), binGridStruct_(std::move(binGridStruct))
{
    populateJsonObject();
}

OsduAbstractBinGrid::OsduAbstractBinGrid(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), binGridStruct_(OsduAbstractBinGridStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractBinGrid::isValidInput() const
{
    return binGridStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractBinGrid::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractBinGridStruct>(binGridStruct_);
}

void
OsduAbstractBinGrid::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractBinGrid::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractBinGridLiterals::BinGridName, binGridStruct_.name);
    setOptionalParameter(OsduAbstractBinGridLiterals::BinGridTypeID, binGridStruct_.typeId);
    setOptionalParameter(OsduAbstractBinGridLiterals::SourceBinGridID, binGridStruct_.sourceId);
    setOptionalParameter(OsduAbstractBinGridLiterals::SourceBinGridAppID, binGridStruct_.sourceAppId);
    setOptionalParameter(OsduAbstractBinGridLiterals::CoveragePercent, binGridStruct_.coveragePercent);
    setOptionalParameter(OsduAbstractBinGridLiterals::BinGridDefinitionMethodTypeID, binGridStruct_.definitionMethodTypeId);
    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduAbstractBinGridLiterals::ABCDBinGridSpatialLocation, binGridStruct_.abcdSpatialLocationStruct);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6TransformationMethod, binGridStruct_.p6TransformationMethod);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinGridOriginI, binGridStruct_.p6BinGridOriginI);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinGridOriginJ, binGridStruct_.p6BinGridOriginJ);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinGridOriginEasting, binGridStruct_.p6BinGridOriginEasting);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinGridOriginNorthing, binGridStruct_.p6BinGridOriginNorthing);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6ScaleFactorOfBinGrid, binGridStruct_.p6ScaleFactorOfBinGrid);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinWidthOnIaxis, binGridStruct_.p6BinWidthOnIaxis);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinWidthOnJaxis, binGridStruct_.p6BinWidthOnJaxis);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6MapGridBearingOfBinGridJaxis, binGridStruct_.p6MapGridBearingOfBinGridJaxis);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinNodeIncrementOnIaxis, binGridStruct_.p6BinNodeIncrementOnIaxis);
    setOptionalParameter(OsduAbstractBinGridLiterals::P6BinNodeIncrementOnJaxis, binGridStruct_.p6BinNodeIncrementOnJaxis);
}

void
OsduAbstractBinGrid::initializeMetadataStruct()
{
    binGridStruct_ = OsduAbstractBinGridStruct(getOsduPartition());
}

void
OsduAbstractBinGrid::populateMetadataStruct()
{
    initializeMetadataStruct();

    binGridStruct_.name = getOptionalParameter<std::string>(OsduAbstractBinGridLiterals::BinGridName);
    binGridStruct_.typeId = getOptionalParameter<std::string>(OsduAbstractBinGridLiterals::BinGridTypeID);
    binGridStruct_.sourceId= getOptionalParameter<int64_t>(OsduAbstractBinGridLiterals::SourceBinGridID);
    binGridStruct_.sourceAppId= getOptionalParameter<std::string>(OsduAbstractBinGridLiterals::SourceBinGridAppID);
    binGridStruct_.coveragePercent = getOptionalParameter<double>(OsduAbstractBinGridLiterals::CoveragePercent);
    binGridStruct_.definitionMethodTypeId= getOptionalParameter<std::string>(OsduAbstractBinGridLiterals::BinGridDefinitionMethodTypeID);
    
    binGridStruct_.abcdSpatialLocationStruct = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduAbstractBinGridLiterals::ABCDBinGridSpatialLocation);

    binGridStruct_.p6TransformationMethod = getOptionalParameter<int64_t>(OsduAbstractBinGridLiterals::P6TransformationMethod);
    binGridStruct_.p6BinGridOriginI = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinGridOriginI);
    binGridStruct_.p6BinGridOriginJ = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinGridOriginJ);
    binGridStruct_.p6BinGridOriginEasting = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinGridOriginEasting);
    binGridStruct_.p6BinGridOriginNorthing = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinGridOriginNorthing);
    binGridStruct_.p6ScaleFactorOfBinGrid = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6ScaleFactorOfBinGrid);
    binGridStruct_.p6BinWidthOnIaxis = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinWidthOnIaxis);
    binGridStruct_.p6BinWidthOnJaxis = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6BinWidthOnJaxis);
    binGridStruct_.p6MapGridBearingOfBinGridJaxis = getOptionalParameter<double>(OsduAbstractBinGridLiterals::P6MapGridBearingOfBinGridJaxis);
    binGridStruct_.p6BinNodeIncrementOnIaxis = getOptionalParameter<int64_t>(OsduAbstractBinGridLiterals::P6BinNodeIncrementOnIaxis);
    binGridStruct_.p6BinNodeIncrementOnJaxis = getOptionalParameter<int64_t>(OsduAbstractBinGridLiterals::P6BinNodeIncrementOnJaxis);
}
} // namespace segysdk::metadata::osdu
