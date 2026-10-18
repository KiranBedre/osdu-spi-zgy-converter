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

#include "../OsduMetaDataUtilities/OsduLineSegment.h"

#include "../OsduMetaDataUtilities/OsduLineSegmentLiterals.h"

#include "../OsduMetaDataUtilities/OsduPointRange.h"
#include "../OsduMetaDataUtilities/OsduShotPointCommonMidPointRatio.h"
#include "../OsduMetaDataUtilities/OsduGenericLabels.h"

namespace segysdk::metadata::osdu
{
OsduLineSegment::OsduLineSegment(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    lineSegmentsStruct_(osduPartition)
{
    /* Empty */
}


OsduLineSegment::OsduLineSegment(OsduLineSegmentStruct lineSegmentsStruct)
  : OsduMetadataBase(lineSegmentsStruct.getPartition()), lineSegmentsStruct_(std::move(lineSegmentsStruct))
{
    populateJsonObject();
}

OsduLineSegment::OsduLineSegment(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), lineSegmentsStruct_(OsduLineSegmentStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduLineSegment::isValidInput() const
{
    return lineSegmentsStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduLineSegment::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduLineSegmentStruct>(lineSegmentsStruct_);
}

void
OsduLineSegment::initializeJson()
{
    clearJsonObject();
}

void
OsduLineSegment::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduLineSegmentLiterals::NavigationLineName, lineSegmentsStruct_.navigationLineName);
    setOptionalParameter(OsduLineSegmentLiterals::SeismicLineName, lineSegmentsStruct_.seismicLineName);
    setOptionalParameter(OsduLineSegmentLiterals::NumberOfLocations, lineSegmentsStruct_.numberOfLocations);

    if (lineSegmentsStruct_.shotPointRangeStruct.has_value()) {
        const auto shotPointRange = OsduPointRange(lineSegmentsStruct_.shotPointRangeStruct.value());
        setParameter(OsduLineSegmentLiterals::ShotPointRange, shotPointRange.getAsJson());
    }

    if (lineSegmentsStruct_.commonMidPointRangeStruct.has_value()) {
        const auto commonMidPointRange = OsduPointRange(lineSegmentsStruct_.commonMidPointRangeStruct.value());
        setParameter(OsduLineSegmentLiterals::CommonMidPointRange, commonMidPointRange.getAsJson());
    }

    if (lineSegmentsStruct_.shotPointCommonMidPointRatioStruct.has_value()) {
        const auto shotPointCommonMidPointRatio = OsduShotPointCommonMidPointRatio(lineSegmentsStruct_.shotPointCommonMidPointRatioStruct.value());
        setParameter(OsduLineSegmentLiterals::ShotPointCommonMidPointRatio, shotPointCommonMidPointRatio.getAsJson());
    }

    if (lineSegmentsStruct_.genericLabelsStruct.has_value()) {
        const auto genericLabels = OsduGenericLabels(lineSegmentsStruct_.genericLabelsStruct.value());
        setParameter(OsduLineSegmentLiterals::GenericLabels, genericLabels.getAsJson());
    }

    setOptionalParameter(OsduLineSegmentLiterals::IsInterpolated, lineSegmentsStruct_.isInterpolated);
    setOptionalParameter(OsduLineSegmentLiterals::IsExtrapolated, lineSegmentsStruct_.isExtrapolated);
}

void
OsduLineSegment::initializeMetadataStruct()
{
    lineSegmentsStruct_ = OsduLineSegmentStruct(getOsduPartition());
}

void
OsduLineSegment::populateMetadataStruct()
{
    initializeMetadataStruct();

    lineSegmentsStruct_.navigationLineName = getOptionalParameter<std::string>(OsduLineSegmentLiterals::NavigationLineName);
    lineSegmentsStruct_.seismicLineName = getParameter<std::string>(OsduLineSegmentLiterals::SeismicLineName);
    lineSegmentsStruct_.numberOfLocations = getOptionalParameter<int>(OsduLineSegmentLiterals::NumberOfLocations);

    lineSegmentsStruct_.shotPointRangeStruct = getOptionalStructParameter<OsduPointRange, OsduPointRangeStruct>(OsduLineSegmentLiterals::ShotPointRange);
    lineSegmentsStruct_.commonMidPointRangeStruct = getOptionalStructParameter<OsduPointRange, OsduPointRangeStruct>(OsduLineSegmentLiterals::CommonMidPointRange);
    lineSegmentsStruct_.shotPointCommonMidPointRatioStruct = getOptionalStructParameter<OsduShotPointCommonMidPointRatio, OsduShotPointCommonMidPointRatioStruct>(OsduLineSegmentLiterals::ShotPointCommonMidPointRatio);
    lineSegmentsStruct_.genericLabelsStruct = getOptionalStructParameter<OsduGenericLabels, OsduGenericLabelsStruct>(OsduLineSegmentLiterals::GenericLabels);

    lineSegmentsStruct_.isExtrapolated = getOptionalParameter<bool>(OsduLineSegmentLiterals::IsExtrapolated);
    lineSegmentsStruct_.isInterpolated = getOptionalParameter<bool>(OsduLineSegmentLiterals::IsInterpolated);
}
} // namespace segysdk::metadata::osdu
