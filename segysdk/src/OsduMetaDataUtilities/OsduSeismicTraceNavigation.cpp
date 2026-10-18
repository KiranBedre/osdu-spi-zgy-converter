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

#include "../OsduMetaDataUtilities/OsduSeismicTraceNavigation.h"

#include "../OsduMetaDataUtilities/OsduAbstractContent.h"
#include "../OsduMetaDataUtilities/OsduLineSegment.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"

#include "../OsduMetaDataUtilities/OsduSeismicTraceNavigationLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicTraceNavigation::OsduSeismicTraceNavigation(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicTraceNavigationStruct_(osduPartition)
{
    /* Empty */
}

OsduSeismicTraceNavigation::OsduSeismicTraceNavigation(OsduSeismicTraceNavigationStruct seismicLineGeometryStruct)
  : OsduMetadataBase(seismicLineGeometryStruct.getPartition()), seismicTraceNavigationStruct_(std::move(seismicLineGeometryStruct))
{
    populateJsonObject();
}

OsduSeismicTraceNavigation::OsduSeismicTraceNavigation(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicTraceNavigationStruct_(OsduSeismicTraceNavigationStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicTraceNavigation::isValidInput() const
{
    return seismicTraceNavigationStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicTraceNavigation::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicTraceNavigationStruct>(seismicTraceNavigationStruct_);
}

void
OsduSeismicTraceNavigation::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicTraceNavigation::populateJsonObject()
{
    initializeJson();

    mergeInJson(OsduAbstractContent(seismicTraceNavigationStruct_.contentStruct).getAsJson());
    setOptionalParameter(OsduSeismicTraceNavigationLiterals::OriginalSeismicLineGeometryID, seismicTraceNavigationStruct_.originalSeismicLineGeometryId);
    setOptionalArrayStructParameter<OsduLineSegment>(OsduSeismicTraceNavigationLiterals::LineSegments, seismicTraceNavigationStruct_.lineSegmentsStruct);
    setOptionalParameter(OsduSeismicTraceNavigationLiterals::TotalNumberOfLocations, seismicTraceNavigationStruct_.totalNumberOfLocations);
    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduSeismicTraceNavigationLiterals::SpatialLocation, seismicTraceNavigationStruct_.spatialLocationStruct);
}

void
OsduSeismicTraceNavigation::initializeMetadataStruct()
{
    seismicTraceNavigationStruct_ = OsduSeismicTraceNavigationStruct(getOsduPartition());
}

void
OsduSeismicTraceNavigation::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicTraceNavigationStruct_.contentStruct = getStructParameter<OsduAbstractContent, OsduAbstractContentStruct>();
    seismicTraceNavigationStruct_.originalSeismicLineGeometryId = getOptionalParameter<std::string>(OsduSeismicTraceNavigationLiterals::OriginalSeismicLineGeometryID);
    seismicTraceNavigationStruct_.lineSegmentsStruct = getOptionalArrayStructParameter<OsduLineSegment, OsduLineSegmentStruct>(OsduSeismicTraceNavigationLiterals::LineSegments);
    seismicTraceNavigationStruct_.totalNumberOfLocations = getOptionalParameter<int>(OsduSeismicTraceNavigationLiterals::TotalNumberOfLocations);
    seismicTraceNavigationStruct_.spatialLocationStruct = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduSeismicTraceNavigationLiterals::SpatialLocation);
}
} // namespace segysdk::metadata::osdu
