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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureStruct.h"


namespace segysdk::metadata::osdu
{
OsduGeoJsonFeatureStruct::OsduGeoJsonFeatureStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition), 
    properties(OsduGeoJsonFeaturePropertiesStruct(osduPartition))
{
    /* Empty */
}

OsduGeoJsonFeatureStruct::OsduGeoJsonFeatureStruct(const OsduGeoJsonFeatureStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    properties(other.properties),
    geometryStruct(other.geometryStruct),
    geometryStructVector(other.geometryStructVector),
    bbox(other.bbox)
{
    /* Empty */
}

OsduGeoJsonFeatureStruct::OsduGeoJsonFeatureStruct(OsduGeoJsonFeatureStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    properties(std::move(other.properties)),
    geometryStruct(std::move(other.geometryStruct)),
    geometryStructVector(std::move(other.geometryStructVector)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduGeoJsonFeatureStruct&
OsduGeoJsonFeatureStruct::operator=(const OsduGeoJsonFeatureStruct& rhs)
{
    return *this = OsduGeoJsonFeatureStruct(rhs);
}

OsduGeoJsonFeatureStruct&
OsduGeoJsonFeatureStruct::operator=(OsduGeoJsonFeatureStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    type = std::move(rhs.type);
    geometryStruct = std::move(rhs.geometryStruct);
    geometryStructVector = std::move(rhs.geometryStructVector);
    properties = std::move(rhs.properties);
    bbox = std::move(rhs.bbox);

    return *this;
}

bool
OsduGeoJsonFeatureStruct::allNodesValid() const
{
    return (geometryValid() or geometriesValid())  and propertiesValid();
}

bool
OsduGeoJsonFeatureStruct::propertiesValid() const
{
    return properties.isValid();
}

bool
OsduGeoJsonFeatureStruct::geometriesValid() const
{
    return geometryStructVector.has_value() and std::all_of(geometryStructVector.value().cbegin(), geometryStructVector.value().cend(), [](const OsduGeoJsonGeometryElementStruct& x) { return x.isValid(); });
}

bool
OsduGeoJsonFeatureStruct::geometryValid() const
{
    return geometryStruct.has_value() and geometryStruct.value().isValid();
}


bool
OsduGeoJsonFeatureStruct::isValid() const
{
    return !type.empty() and allNodesValid();
}

std::vector<std::string>
OsduGeoJsonFeatureStruct::performAttributeValidityScan() const
{

    if (isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduGeoJsonFeatureStruct::isValid() failed.");
    if (type.empty()) {
        result.emplace_back("OsduGeoJsonFeatureStruct::type cannot be empty.");
    }
    
    if (geometryStructVector.has_value() and !geometriesValid()) {
        result.emplace_back("OsduGeoJsonFeatureStruct::geometriesValid() failed.");
        for (const auto& g : geometryStructVector.value()) {
            if (!g.isValid()) {
                appendStringVector(result, g.performAttributeValidityScan());
            }
        }
    }

    if (geometryStruct.has_value() and !geometryValid()) {
        result.emplace_back("OsduGeoJsonFeatureStruct::geometryValid() failed.");
        appendStringVector(result, geometryStruct.value().performAttributeValidityScan());
    }

    if (!propertiesValid()) {
        result.emplace_back("OsduGeoJsonFeatureStruct::propertiesValid() failed.");
        appendStringVector(result, properties.performAttributeValidityScan());
    }
   
    return result;
}

void
OsduGeoJsonFeatureStruct::propagatePartition(const std::string& partition)
{
    properties.setPartition(partition);

    if (geometryStruct.has_value()) {
        geometryStruct.value().setPartition(partition);
    }
    if (geometryStructVector.has_value()) {
        for (auto& geometry : geometryStructVector.value()) {
            geometry.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
