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

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractSpatialLocationStruct::OsduAbstractSpatialLocationStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractSpatialLocationStruct::OsduAbstractSpatialLocationStruct(const OsduAbstractSpatialLocationStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    spatialLocationCoordinatesDate(other.spatialLocationCoordinatesDate),
    quantitativeAccuracyBandID(other.quantitativeAccuracyBandID),
    qualitativeSpatialAccuracyTypeID(other.qualitativeSpatialAccuracyTypeID),
    coordinateQualityCheckPerformedBy(other.coordinateQualityCheckPerformedBy),
    coordinateQualityCheckDateTime(other.coordinateQualityCheckDateTime),
    coordinateQualityCheckRemarks(other.coordinateQualityCheckRemarks),
    asIngestedCoordinates(other.asIngestedCoordinates),
    wgs84Coordinates(other.wgs84Coordinates),
    appliedOperations(other.appliedOperations),
    spatialParameterTypeID(other.spatialParameterTypeID),
    spatialGeometryTypeID(other.spatialGeometryTypeID)
{
    /* Empty */
}

OsduAbstractSpatialLocationStruct::OsduAbstractSpatialLocationStruct(OsduAbstractSpatialLocationStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    spatialLocationCoordinatesDate(std::move(other.spatialLocationCoordinatesDate)),
    quantitativeAccuracyBandID(std::move(other.quantitativeAccuracyBandID)),
    qualitativeSpatialAccuracyTypeID(std::move(other.qualitativeSpatialAccuracyTypeID)),
    coordinateQualityCheckPerformedBy(std::move(other.coordinateQualityCheckPerformedBy)),
    coordinateQualityCheckDateTime(std::move(other.coordinateQualityCheckDateTime)),
    coordinateQualityCheckRemarks(std::move(other.coordinateQualityCheckRemarks)),
    asIngestedCoordinates(std::move(other.asIngestedCoordinates)),
    wgs84Coordinates(std::move(other.wgs84Coordinates)),
    appliedOperations(std::move(other.appliedOperations)),
    spatialParameterTypeID(std::move(other.spatialParameterTypeID)),
    spatialGeometryTypeID(std::move(other.spatialGeometryTypeID))
{
    /* Empty */
}

OsduAbstractSpatialLocationStruct&
OsduAbstractSpatialLocationStruct::operator=(const OsduAbstractSpatialLocationStruct& rhs)
{
    return *this = OsduAbstractSpatialLocationStruct(rhs);
}

OsduAbstractSpatialLocationStruct&
OsduAbstractSpatialLocationStruct::operator=(OsduAbstractSpatialLocationStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    spatialLocationCoordinatesDate = std::move(rhs.spatialLocationCoordinatesDate);
    quantitativeAccuracyBandID = std::move(rhs.quantitativeAccuracyBandID);
    qualitativeSpatialAccuracyTypeID = std::move(rhs.qualitativeSpatialAccuracyTypeID);
    coordinateQualityCheckPerformedBy = std::move(rhs.coordinateQualityCheckPerformedBy);
    coordinateQualityCheckDateTime = std::move(rhs.coordinateQualityCheckDateTime);
    coordinateQualityCheckRemarks = std::move(rhs.coordinateQualityCheckRemarks);
    asIngestedCoordinates = std::move(rhs.asIngestedCoordinates);
    wgs84Coordinates = std::move(rhs.wgs84Coordinates);
    appliedOperations = std::move(rhs.appliedOperations);
    spatialParameterTypeID = std::move(rhs.spatialParameterTypeID);
    spatialGeometryTypeID = std::move(rhs.spatialGeometryTypeID);

    return *this;
}

bool
OsduAbstractSpatialLocationStruct::quantitativeAccuracyBandIDisValid() const
{
    return !quantitativeAccuracyBandID.has_value() or isValidOSDUReferenceIdString(quantitativeAccuracyBandID.value());
}

bool
OsduAbstractSpatialLocationStruct::qualitativeSpatialAccuracyTypeIDisValid() const
{
    return !qualitativeSpatialAccuracyTypeID.has_value() or isValidOSDUReferenceIdString(qualitativeSpatialAccuracyTypeID.value());
}

bool
OsduAbstractSpatialLocationStruct::spatialParameterTypeIDisValid() const
{
    return !spatialParameterTypeID.has_value() or isValidOSDUReferenceIdString(spatialParameterTypeID.value());
}

bool
OsduAbstractSpatialLocationStruct::spatialGeometryTypeIDisValid() const
{
    return !spatialGeometryTypeID.has_value() or isValidOSDUReferenceIdString(spatialGeometryTypeID.value());
}

bool
OsduAbstractSpatialLocationStruct::asIngestedCoordinatesIsValid() const
{
    return !asIngestedCoordinates.has_value() or asIngestedCoordinates.value().isValid();
}

bool
OsduAbstractSpatialLocationStruct::wgs84CoordinatesIsValid() const
{
    return !wgs84Coordinates.has_value() or wgs84Coordinates.value().isValid();
}

bool
OsduAbstractSpatialLocationStruct::isValid() const
{
    return quantitativeAccuracyBandIDisValid() and qualitativeSpatialAccuracyTypeIDisValid() and spatialParameterTypeIDisValid() and spatialGeometryTypeIDisValid() and asIngestedCoordinatesIsValid() and wgs84CoordinatesIsValid();
}

std::vector<std::string>
OsduAbstractSpatialLocationStruct::performAttributeValidityScan() const
{
    std::vector<std::string> result;
    if (!isValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct::isValid() failed.");
    }

    if (quantitativeAccuracyBandID.has_value() and !quantitativeAccuracyBandIDisValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: quantitativeAccuracyBandID " + quantitativeAccuracyBandID.value() + " is not a valid OSDU reference string");
    }
    if (qualitativeSpatialAccuracyTypeID.has_value() and !qualitativeSpatialAccuracyTypeIDisValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: qualitativeSpatialAccuracyTypeID " + qualitativeSpatialAccuracyTypeID.value() + " is not a valid OSDU reference string");
    }
    if (spatialParameterTypeID.has_value() and !spatialParameterTypeIDisValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: spatialParameterTypeID " + spatialParameterTypeID.value() + " is not a valid OSDU reference string");
    }
    if (spatialGeometryTypeID .has_value() and !spatialParameterTypeIDisValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: spatialGeometryTypeID " + spatialGeometryTypeID.value() + " is not a valid OSDU reference string");
    }
    if (asIngestedCoordinates.has_value() and !asIngestedCoordinatesIsValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: asIngestedCoordinatesIsValid is not valid");
        appendStringVector(result, asIngestedCoordinates.value().performAttributeValidityScan());
    }
    if (wgs84Coordinates.has_value() and !wgs84CoordinatesIsValid()) {
        result.emplace_back("OsduAbstractSpatialLocationStruct: wgs84CoordinatesIsValid is not valid");
        appendStringVector(result, wgs84Coordinates.value().performAttributeValidityScan());
    }
    return result;
}

} // namespace segysdk::metadata::osdu
