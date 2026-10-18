/*
Copyright 2023 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollection.h"
#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollection.h"

namespace segysdk::metadata::osdu
{
OsduAbstractSpatialLocation::OsduAbstractSpatialLocation(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), spatialLocationStruct_(OsduAbstractSpatialLocationStruct(osduPartition)) {}

OsduAbstractSpatialLocation::OsduAbstractSpatialLocation(const OsduAbstractSpatialLocation& other)
    : OsduMetadataBase(other.getOsduPartition()), spatialLocationStruct_(other.spatialLocationStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractSpatialLocation::OsduAbstractSpatialLocation(OsduAbstractSpatialLocation&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), spatialLocationStruct_(std::move(other.spatialLocationStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractSpatialLocation&
OsduAbstractSpatialLocation::operator=(const OsduAbstractSpatialLocation& rhs)
{
    return *this = OsduAbstractSpatialLocation(rhs);
}

OsduAbstractSpatialLocation&
OsduAbstractSpatialLocation::operator=(OsduAbstractSpatialLocation&& rhs) noexcept
{
    spatialLocationStruct_ = std::move(rhs.spatialLocationStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    return *this;
}

OsduAbstractSpatialLocation::OsduAbstractSpatialLocation(OsduAbstractSpatialLocationStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), spatialLocationStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduAbstractSpatialLocation::OsduAbstractSpatialLocation(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), spatialLocationStruct_(OsduAbstractSpatialLocationStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduAbstractSpatialLocation::isValidInput() const
{
    return spatialLocationStruct_.isValid();
}

void
OsduAbstractSpatialLocation::initializeJson()
{
    removeMember(OsduAbstractSpatialLocationLiterals::AppliedOperations);
    removeMember(OsduAbstractSpatialLocationLiterals::AsIngestedCoordinates);
    removeMember(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckDateTime);
    removeMember(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckPerformedBy);
    removeMember(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckRemarks);
    removeMember(OsduAbstractSpatialLocationLiterals::QualitativeSpatialAccuracyTypeID);
    removeMember(OsduAbstractSpatialLocationLiterals::QuantitativeAccuracyBandID);
    removeMember(OsduAbstractSpatialLocationLiterals::SpatialGeometryTypeID);
    removeMember(OsduAbstractSpatialLocationLiterals::SpatialLocationCoordinatesDate);
    removeMember(OsduAbstractSpatialLocationLiterals::SpatialLocationCoordinatesDate);
    removeMember(OsduAbstractSpatialLocationLiterals::SpatialLocationCoordinatesDate);
}

void
OsduAbstractSpatialLocation::populateJsonObject()
{
    clearJsonObject();

    setOptionalParameter(OsduAbstractSpatialLocationLiterals::SpatialLocationCoordinatesDate, spatialLocationStruct_.spatialLocationCoordinatesDate);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::QuantitativeAccuracyBandID, spatialLocationStruct_.quantitativeAccuracyBandID);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::QualitativeSpatialAccuracyTypeID, spatialLocationStruct_.qualitativeSpatialAccuracyTypeID);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckPerformedBy, spatialLocationStruct_.coordinateQualityCheckPerformedBy);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckDateTime, spatialLocationStruct_.coordinateQualityCheckDateTime);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::SpatialParameterTypeID, spatialLocationStruct_.spatialParameterTypeID);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::SpatialParameterTypeID, spatialLocationStruct_.spatialParameterTypeID);
    setOptionalParameter(OsduAbstractSpatialLocationLiterals::SpatialGeometryTypeID, spatialLocationStruct_.spatialGeometryTypeID);

    if (spatialLocationStruct_.coordinateQualityCheckRemarks.has_value()) {
        InitializeJsonArray(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckRemarks);
        for (const auto& txt : spatialLocationStruct_.coordinateQualityCheckRemarks.value()) {
            appendArrayElement(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckRemarks, txt);
        }
    }
    if (spatialLocationStruct_.asIngestedCoordinates.has_value()) {
        OsduAbstractAnyCrsFeatureCollection asIngested(spatialLocationStruct_.asIngestedCoordinates.value());
        setParameter(OsduAbstractSpatialLocationLiterals::AsIngestedCoordinates, asIngested.getAsJson());
    }
    if (spatialLocationStruct_.wgs84Coordinates.has_value()) {
        OsduAbstractFeatureCollection wgs84(spatialLocationStruct_.wgs84Coordinates.value());
        setParameter(OsduAbstractSpatialLocationLiterals::Wgs84Coordinates, wgs84.getAsJson());
    }
    if (spatialLocationStruct_.appliedOperations.has_value()) {
        InitializeJsonArray(OsduAbstractSpatialLocationLiterals::AppliedOperations);
        for (const auto& txt : spatialLocationStruct_.appliedOperations.value()) {
            appendArrayElement(OsduAbstractSpatialLocationLiterals::AppliedOperations, txt);
        }
    }
}

void
OsduAbstractSpatialLocation::initializeMetadataStruct()
{
    spatialLocationStruct_ = OsduAbstractSpatialLocationStruct(getOsduPartition());
}

std::shared_ptr<OsduAbstractAnyCrsFeatureCollectionStruct> 
OsduAbstractSpatialLocation::getOsduAbstractAnyCrsFeatureCollectionStruct(const Json::Value& json) const
{
    OsduAbstractAnyCrsFeatureCollection asIngested(json, getOsduPartition());
    return std::reinterpret_pointer_cast<OsduAbstractAnyCrsFeatureCollectionStruct>(asIngested.getMetaDataStruct());

}

std::shared_ptr<OsduAbstractFeatureCollectionStruct>
OsduAbstractSpatialLocation::getOsduAbstractFeatureCollectionStruct(const Json::Value& json) const
{
    OsduAbstractFeatureCollection wgs84(json, getOsduPartition());
    return std::reinterpret_pointer_cast<OsduAbstractFeatureCollectionStruct>(wgs84.getMetaDataStruct());
}

void
OsduAbstractSpatialLocation::populateMetadataStruct()
{
    initializeMetadataStruct();

    spatialLocationStruct_.spatialLocationCoordinatesDate = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::SpatialLocationCoordinatesDate);
    spatialLocationStruct_.quantitativeAccuracyBandID = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::QuantitativeAccuracyBandID);
    spatialLocationStruct_.qualitativeSpatialAccuracyTypeID = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::QualitativeSpatialAccuracyTypeID);
    spatialLocationStruct_.coordinateQualityCheckPerformedBy = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckPerformedBy);
    spatialLocationStruct_.coordinateQualityCheckDateTime = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckDateTime);
    spatialLocationStruct_.spatialParameterTypeID = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::SpatialParameterTypeID);
    spatialLocationStruct_.spatialGeometryTypeID = getOptionalParameter<std::string>(OsduAbstractSpatialLocationLiterals::SpatialGeometryTypeID);

    if (isMember(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckRemarks)) {
        spatialLocationStruct_.coordinateQualityCheckRemarks = getAllArrayElements<std::string>(OsduAbstractSpatialLocationLiterals::CoordinateQualityCheckRemarks);
    }
    if (isMember(OsduAbstractSpatialLocationLiterals::AsIngestedCoordinates)) {
        const Json::Value json = getParameter<Json::Value>(OsduAbstractSpatialLocationLiterals::AsIngestedCoordinates);
        const std::shared_ptr<OsduAbstractAnyCrsFeatureCollectionStruct> ptr = getOsduAbstractAnyCrsFeatureCollectionStruct(json);
        if (ptr != nullptr) {
            spatialLocationStruct_.asIngestedCoordinates = *ptr;
        }
    }
    if (isMember(OsduAbstractSpatialLocationLiterals::Wgs84Coordinates)) {
        const Json::Value json = getParameter<Json::Value>(OsduAbstractSpatialLocationLiterals::Wgs84Coordinates);
        const std::shared_ptr<OsduAbstractFeatureCollectionStruct> ptr = getOsduAbstractFeatureCollectionStruct(json);
        if (ptr != nullptr) {
            spatialLocationStruct_.wgs84Coordinates = *ptr;
        }
    }
    if (isMember(OsduAbstractSpatialLocationLiterals::AppliedOperations)) {
        spatialLocationStruct_.appliedOperations = getAllArrayElements<std::string>(OsduAbstractSpatialLocationLiterals::AppliedOperations);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractSpatialLocation::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractSpatialLocationStruct>(spatialLocationStruct_);
}

} // namespace segysdk::metadata::osdu
