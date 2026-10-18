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

#include "SegyServiceSessionImpl.h"

#include "../Logger/Logger.h"
#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometry.h"
#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceData.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceDataStruct.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceNavigation.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"

namespace segysdk::api
{

SegyServiceSessionImpl::SegyServiceSessionImpl(std::string seismicTraceData)
  : seismicTraceData_(seismicTraceData)
{
}

std::vector<TraceNavigationData>
SegyServiceSessionImpl::getTraceNavigation() const
{
    using namespace metadata::osdu;
    const auto traceDataStructPtr = std::reinterpret_pointer_cast<OsduSeismicTraceDataStruct>(getSeismicTraceData().getMetaDataStruct());
    if (not traceDataStructPtr) {
        return {};
    }

    if (not traceDataStructPtr->seismicTraceDataSpecificProperties.has_value()) {
        return {};
    }

    const std::string dimensionality = traceDataStructPtr->seismicTraceDataSpecificProperties.value().seismicTraceDataDimensionalityTypeID.value_or("");
    if (not isDimensionalityTypeId2D(dimensionality)) {
        return {};
    }

    if (traceDataStructPtr->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.has_value()) {

        if (traceDataStructPtr->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.has_value()) {
            const auto& artefacts = traceDataStructPtr->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.value();
            for (const auto& artefact : artefacts) {
                if (not (artefact.resourceID.has_value() and artefact.resourceKind.has_value())) {
                    continue;
                }
                if (artefact.resourceKind.value() == OsduFileSeismicLineGeometryStruct::getOsduKind()) {
                    const auto& partition = artefact.getPartition();
                    auto lineGeometry = OsduFileSeismicLineGeometry(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(artefact.resourceID.value()), partition);
                    auto lineGeometryStruct = std::reinterpret_pointer_cast<OsduFileSeismicLineGeometryStruct>(lineGeometry.getMetaDataStruct());
                    if (not lineGeometryStruct or not lineGeometryStruct->fileSeismicLineGeometryDataStruct.has_value()) {
                        continue;
                    }
                    if (not lineGeometryStruct->fileSeismicLineGeometryDataStruct.value().fileStruct.has_value() or not lineGeometryStruct->fileSeismicLineGeometryDataStruct.value().fileStruct.value().fileSourceInfo.has_value()) {
                        continue;
                    }
                    const auto& fileStruct = lineGeometryStruct->fileSeismicLineGeometryDataStruct.value().fileStruct.value().fileSourceInfo.value();
                    const auto& traceNavigationPath = fileStruct.fileSource;
                    Json::Value traceNavigation = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(traceNavigationPath);
                    return extractTraceNavigation(traceNavigation, partition);
                }
            }
        }
    }
    return {};
}

segysdk::metadata::osdu::OsduSeismicTraceData
SegyServiceSessionImpl::getSeismicTraceData() const
{
    const std::string osduPartition = segysdk::utilities::Environment::instance().osduDataPartitionId();
    return segysdk::metadata::osdu::OsduSeismicTraceData(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(seismicTraceData_), osduPartition);
}

bool
SegyServiceSessionImpl::isDimensionalityTypeId2D(const std::string& dimensionalityTypeId) const
{
    size_t pos = dimensionalityTypeId.find("SeismicTraceDataDimensionalityType:");
    if (pos == std::string::npos) {
        return false;
    }
    const std::string dim = dimensionalityTypeId.substr(pos);
    pos = dim.find(":");
    const std::string dimension = dim.substr(pos + 1, dim.rfind(":") - pos - 1);

    return dimension == "2D";
    return false;
}

std::vector<TraceNavigationData>
SegyServiceSessionImpl::extractTraceNavigation(const Json::Value& traceData, const std::string& osduPartition) const
{
    using namespace segysdk::metadata::osdu;
    auto traceNavigation = OsduSeismicTraceNavigation(traceData, osduPartition);
    auto traceNavigationStructPtr = std::reinterpret_pointer_cast<OsduSeismicTraceNavigationStruct>(traceNavigation.getMetaDataStruct());
    const size_t tracesNumb = traceNavigationStructPtr->totalNumberOfLocations.value_or(0);
    if (not traceNavigationStructPtr) {
        return {};
    }

    if (not traceNavigationStructPtr->spatialLocationStruct.has_value()) {
        return {};
    }
    const auto& spatialStruct = traceNavigationStructPtr->spatialLocationStruct.value();
    // TODO: should be here:  spatialStruct.asIngestedCoordinates
    if (not spatialStruct.wgs84Coordinates.has_value()) {
        return {};
    }
    for (const auto& feature : spatialStruct.wgs84Coordinates.value().features) {
        if (not feature.geometryStruct.has_value() or not feature.properties.points2DProperties.has_value()) {
            continue;
        }
        if (not feature.geometryStruct.value().coordinates_line.has_value()) {
            continue;
        }
        const auto& coordinates = feature.geometryStruct.value().coordinates_line.value();
        const auto& midPoints = feature.properties.points2DProperties.value().commonMidPointNumbers;
        const auto& shotPoints = feature.properties.points2DProperties.value().shotpointNumbers;
        const bool areEqual = coordinates.size() == midPoints.size() and midPoints.size() == shotPoints.size();
        if (not areEqual or (tracesNumb != 0 and tracesNumb != coordinates.size())) {
            SEGYLOG_REPORT_ERROR("segyServiceSessio; Number of coordinates/midPoints/shotPoints and traces are not equal in TraceNavigation Data");
            continue;
        }
        std::vector<TraceNavigationData> result;
        result.reserve(coordinates.size());
        for (size_t i = 0; i < coordinates.size(); ++i) {
            result.emplace_back(midPoints[i], shotPoints[i], coordinates[i].at(0), coordinates[i].at(1));
        }
        return result;
    }
    return {};
}

} // namespace segysdk::api
