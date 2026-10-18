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

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonFeatureStruct.h"


namespace segysdk::metadata::osdu
{
OsduAnyCrsGeoJsonFeatureStruct::OsduAnyCrsGeoJsonFeatureStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition),
    properties(OsduGeoJsonFeaturePropertiesStruct(osduPartition)),
    geometry(OsduAnyCrsGeoJsonGeometryElementStruct(osduPartition))
{
    /* Empty */
}

OsduAnyCrsGeoJsonFeatureStruct::OsduAnyCrsGeoJsonFeatureStruct(const OsduAnyCrsGeoJsonFeatureStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    properties(other.properties),
    geometry(other.geometry),
    geometries(other.geometries),
    bbox(other.bbox)
{
    /* Empty */
}

OsduAnyCrsGeoJsonFeatureStruct::OsduAnyCrsGeoJsonFeatureStruct(OsduAnyCrsGeoJsonFeatureStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    properties(std::move(other.properties)),
    geometry(std::move(other.geometry)),
    geometries(std::move(other.geometries)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduAnyCrsGeoJsonFeatureStruct&
OsduAnyCrsGeoJsonFeatureStruct::operator=(const OsduAnyCrsGeoJsonFeatureStruct& rhs)
{
    return *this = OsduAnyCrsGeoJsonFeatureStruct(rhs);
}

OsduAnyCrsGeoJsonFeatureStruct&
OsduAnyCrsGeoJsonFeatureStruct::operator=(OsduAnyCrsGeoJsonFeatureStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    type = std::move(rhs.type);
    properties = std::move(rhs.properties);
    geometry = std::move(rhs.geometry);
    geometries = std::move(rhs.geometries);
    bbox = std::move(rhs.bbox);

    return *this;
}

bool
OsduAnyCrsGeoJsonFeatureStruct::allNodesValid() const
{
    return geometriesValid() and propertiesValid();
}

bool
OsduAnyCrsGeoJsonFeatureStruct::propertiesValid() const
{
    return properties.isValid();
}

bool
OsduAnyCrsGeoJsonFeatureStruct::geometriesValid() const
{
    if (geometry.has_value()) {
        return geometry.value().isValid();
    }
    if (geometries.has_value()) {
        return std::all_of(geometries.value().cbegin(), geometries.value().cend(), [](const OsduAnyCrsGeoJsonGeometryElementStruct& x) { return x.isValid(); });
    }

    return false;
}

bool
OsduAnyCrsGeoJsonFeatureStruct::isValid() const
{
    return !type.empty() and allNodesValid();
}

std::vector<std::string>
OsduAnyCrsGeoJsonFeatureStruct::performAttributeValidityScan() const
{

    if (isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduAnyCrsGeoJsonFeatureStruct::isValid() failed.");
    if (type.empty()) {
        result.emplace_back("OsduAnyCrsGeoJsonFeatureStruct::type cannot be empty.");
    }
    if (geometries.has_value() and !geometriesValid()) {
        result.emplace_back("OsduAnyCrsGeoJsonFeatureStruct::geometriesValid() failed.");
        for (const auto& g : geometries.value()) {
            if (!g.isValid()) {
                appendStringVector(result, g.performAttributeValidityScan());
            }
        }
    }
    if (geometry.has_value() and !geometry.value().isValid()) {
        result.emplace_back("OsduAnyCrsGeoJsonFeatureStruct::geometryValid() failed.");
        appendStringVector(result, geometry.value().performAttributeValidityScan());
    }

    if (!properties.isValid()) {
        result.emplace_back("OsduAnyCrsGeoJsonFeatureStruct::propertiesValid() failed.");
        appendStringVector(result, properties.performAttributeValidityScan());
    }

    return result;
}

void
OsduAnyCrsGeoJsonFeatureStruct::propagatePartition(const std::string& partition)
{
    if (geometry.has_value()) {
        geometry.value().setPartition(partition);
    }
    if (geometries.has_value()) {
        for (OsduAnyCrsGeoJsonGeometryElementStruct& g : geometries.value()) {
            g.setPartition(partition);
        }
    }
    properties.setPartition(partition);
}

} // namespace segysdk::metadata::osdu
