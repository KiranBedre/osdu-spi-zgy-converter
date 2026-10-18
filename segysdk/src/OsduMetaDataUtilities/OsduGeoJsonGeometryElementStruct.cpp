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

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElementStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduGeoJsonGeometryElementStruct::OsduGeoJsonGeometryElementStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGeoJsonGeometryElementStruct::OsduGeoJsonGeometryElementStruct(const OsduGeoJsonGeometryElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    coordinates_point(other.coordinates_point),
    coordinates_line(other.coordinates_line),
    coordinates_polygon(other.coordinates_polygon),
    coordinates_multipolygon(other.coordinates_multipolygon),
    geometries(other.geometries),
    bbox(other.bbox)
{
    /* Empty */
}

OsduGeoJsonGeometryElementStruct::OsduGeoJsonGeometryElementStruct(OsduGeoJsonGeometryElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    coordinates_point(std::move(other.coordinates_point)),
    coordinates_line(std::move(other.coordinates_line)),
    coordinates_polygon(std::move(other.coordinates_polygon)),
    coordinates_multipolygon(std::move(other.coordinates_multipolygon)),
    geometries(std::move(other.geometries)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduGeoJsonGeometryElementStruct&
OsduGeoJsonGeometryElementStruct::operator=(const OsduGeoJsonGeometryElementStruct& rhs)
{
    return *this = OsduGeoJsonGeometryElementStruct(rhs);
}

OsduGeoJsonGeometryElementStruct&
OsduGeoJsonGeometryElementStruct::operator=(OsduGeoJsonGeometryElementStruct&& rhs) noexcept
{
    type = std::move(rhs.type);
    coordinates_point = std::move(rhs.coordinates_point);
    coordinates_line = std::move(rhs.coordinates_line);
    coordinates_polygon = std::move(rhs.coordinates_polygon);
    coordinates_multipolygon = std::move(rhs.coordinates_multipolygon);
    geometries = std::move(rhs.geometries);
    bbox = std::move(rhs.bbox);

    setPartition(rhs.getPartition());

    return *this;
}


bool
OsduGeoJsonGeometryElementStruct::isValid() const
{
    return !type.empty() and CoordinatesValid(coordinates_point, coordinates_line, coordinates_polygon, coordinates_multipolygon) and GeometriesValid();
}



bool
OsduGeoJsonGeometryElementStruct::GeometriesValid() const
{
    if (!geometries.has_value() ) {
        return true;
    }
    return std::all_of(geometries.value().cbegin(), geometries.value().cend(), [](const OsduGeoJsonGeometryElementStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduGeoJsonGeometryElementStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> result{"OsduGeoJsonGeometryElementStruct::isValid() failed."};
    

    if (type.empty()) {
        result.emplace_back("OsduGeoJsonGeometryElementStruct::type must have a value.");
    } 

    if (!CoordinatesValid(coordinates_point, coordinates_line, coordinates_polygon, coordinates_multipolygon)) {
        result.emplace_back("OsduGeoJsonGeometryElementStruct::coordinatesValid(): type = " + type);
        result.emplace_back("OsduGeoJsonGeometryElementStruct::coordinates must have valid values matching the type.");
    }

    if (geometries.has_value()) {
        int geometryNo = 0;
        for (const auto& geometry : geometries.value()) {
            if (!geometry.isValid()) {
                result.emplace_back("OsduGeoJsonGeometryElementStruct::Geometries element " + std::to_string(geometryNo++) + " not valid.");
                appendStringVector(result, geometry.performAttributeValidityScan());
            }
        }
    }
    return result;
}

void
OsduGeoJsonGeometryElementStruct::propagatePartition(const std::string& partition)
{
    if (geometries.has_value()) {
        for (auto& geometry : geometries.value()) {
            geometry.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
