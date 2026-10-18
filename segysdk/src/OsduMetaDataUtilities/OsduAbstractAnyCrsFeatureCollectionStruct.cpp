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

#include "../OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollectionStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractAnyCrsFeatureCollectionStruct::OsduAbstractAnyCrsFeatureCollectionStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

OsduAbstractAnyCrsFeatureCollectionStruct::OsduAbstractAnyCrsFeatureCollectionStruct(const OsduAbstractAnyCrsFeatureCollectionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    coordinateReferenceSystemID(other.coordinateReferenceSystemID),
    verticalCoordinateReferenceSystemID(other.verticalCoordinateReferenceSystemID),
    verticalUnitID(other.verticalUnitID),
    persistableReferenceCrs(other.persistableReferenceCrs),
    persistableReferenceVerticalCrs(other.persistableReferenceVerticalCrs),
    persistableReferenceUnitZ(other.persistableReferenceUnitZ),
    features(other.features),
    bbox(other.bbox)
{
    /* Empty */
}

OsduAbstractAnyCrsFeatureCollectionStruct::OsduAbstractAnyCrsFeatureCollectionStruct(OsduAbstractAnyCrsFeatureCollectionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    coordinateReferenceSystemID(std::move(other.coordinateReferenceSystemID)),
    verticalCoordinateReferenceSystemID(std::move(other.verticalCoordinateReferenceSystemID)),
    verticalUnitID(std::move(other.verticalUnitID)),
    persistableReferenceCrs(std::move(other.persistableReferenceCrs)),
    persistableReferenceVerticalCrs(std::move(other.persistableReferenceVerticalCrs)),
    persistableReferenceUnitZ(std::move(other.persistableReferenceUnitZ)),
    features(std::move(other.features)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduAbstractAnyCrsFeatureCollectionStruct&
OsduAbstractAnyCrsFeatureCollectionStruct::operator=(const OsduAbstractAnyCrsFeatureCollectionStruct& rhs)
{
    return *this = OsduAbstractAnyCrsFeatureCollectionStruct(rhs);
}

OsduAbstractAnyCrsFeatureCollectionStruct&
OsduAbstractAnyCrsFeatureCollectionStruct::operator=(OsduAbstractAnyCrsFeatureCollectionStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    type = std::move(rhs.type);
    coordinateReferenceSystemID = std::move(rhs.coordinateReferenceSystemID);
    verticalCoordinateReferenceSystemID = std::move(rhs.verticalCoordinateReferenceSystemID);
    verticalUnitID = std::move(rhs.verticalUnitID);
    persistableReferenceCrs = std::move(rhs.persistableReferenceCrs);
    persistableReferenceVerticalCrs = std::move(rhs.persistableReferenceVerticalCrs);
    persistableReferenceUnitZ = std::move(rhs.persistableReferenceUnitZ);
    features = std::move(rhs.features);
    bbox = std::move(rhs.bbox);
    return *this;
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::isValid() const
{
    return !type.empty() and coordinateReferenceSystemIDValid() and persistableReferenceCrsValid() and verticalCoordinateReferenceSystemIDValid() and verticalUnitIDValid() and featuresValid();
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::coordinateReferenceSystemIDValid() const
{
    return !coordinateReferenceSystemID.has_value() or isValidOSDUCoordinateReferenceIdString(coordinateReferenceSystemID.value());
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::verticalCoordinateReferenceSystemIDValid() const
{
    return !verticalCoordinateReferenceSystemID.has_value() or isValidOSDUCoordinateReferenceIdString(verticalCoordinateReferenceSystemID.value());
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::verticalUnitIDValid() const
{
    return !verticalUnitID.has_value() or isValidOSDUReferenceIdString(verticalUnitID.value());
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::persistableReferenceCrsValid() const
{
    return !persistableReferenceCrs.empty();
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::persistableReferenceVerticalCrsValid() const
{
    return !persistableReferenceVerticalCrs.has_value() || !persistableReferenceVerticalCrs.value().empty();
}

bool
OsduAbstractAnyCrsFeatureCollectionStruct::featuresValid() const
{
    return std::all_of(features.cbegin(), features.cend(), [](const OsduAnyCrsGeoJsonFeatureStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduAbstractAnyCrsFeatureCollectionStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct::isValid() failed.");

    if (type.empty()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct::type cannot be empty");
    }
    if (!coordinateReferenceSystemIDValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: coordinateReferenceSystemIDValid() failed.");
    }
    if (!verticalCoordinateReferenceSystemIDValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: verticalCoordinateReferenceSystemIDValid() failed.");
    }
    if (!verticalUnitIDValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: verticalUnitIDValid() failed.");
    }
    if (!persistableReferenceCrsValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: persistableReferenceCrsValid() failed.");
    }
    if (!persistableReferenceVerticalCrsValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: persistableReferenceVerticalCrsValid() failed.");
    }
    if (!featuresValid()) {
        result.emplace_back("OsduAbstractAnyCrsFeatureCollectionStruct:: featuresValid() failed.");
        for (const auto& feature : features) {
            appendStringVector(result, feature.performAttributeValidityScan());
        }
    }
    return result;
}

void
OsduAbstractAnyCrsFeatureCollectionStruct::propagatePartition(const std::string& partition)
{
    for (auto& feature : features) {
        feature.setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
