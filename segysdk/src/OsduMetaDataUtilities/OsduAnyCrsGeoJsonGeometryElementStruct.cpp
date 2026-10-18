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

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonGeometryElementStruct.h"


namespace segysdk::metadata::osdu
{
OsduAnyCrsGeoJsonGeometryElementStruct::OsduAnyCrsGeoJsonGeometryElementStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAnyCrsGeoJsonGeometryElementStruct::OsduAnyCrsGeoJsonGeometryElementStruct(const OsduAnyCrsGeoJsonGeometryElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()), OsduMetadataGeoJsonGeometryElementStructBase(),
    type(other.type),
    coordinates_point(other.coordinates_point),
    coordinates_line(other.coordinates_line),
    coordinates_polygon(other.coordinates_polygon),
    coordinates_multipolygon(other.coordinates_multipolygon),
    bbox(other.bbox)
{
    /* Empty */
}

OsduAnyCrsGeoJsonGeometryElementStruct::OsduAnyCrsGeoJsonGeometryElementStruct(OsduAnyCrsGeoJsonGeometryElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    coordinates_point(std::move(other.coordinates_point)),
    coordinates_line(std::move(other.coordinates_line)),
    coordinates_polygon(std::move(other.coordinates_polygon)),
    coordinates_multipolygon(std::move(other.coordinates_multipolygon)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduAnyCrsGeoJsonGeometryElementStruct&
OsduAnyCrsGeoJsonGeometryElementStruct::operator=(const OsduAnyCrsGeoJsonGeometryElementStruct& rhs)
{
    return *this = OsduAnyCrsGeoJsonGeometryElementStruct(rhs);
}

OsduAnyCrsGeoJsonGeometryElementStruct&
OsduAnyCrsGeoJsonGeometryElementStruct::operator=(OsduAnyCrsGeoJsonGeometryElementStruct&& rhs) noexcept
{
    type = std::move(rhs.type);
    coordinates_point = std::move(rhs.coordinates_point);
    coordinates_line = std::move(rhs.coordinates_line);
    coordinates_polygon = std::move(rhs.coordinates_polygon);
    coordinates_multipolygon = std::move(rhs.coordinates_multipolygon);
    bbox = std::move(rhs.bbox);

    setPartition(rhs.getPartition());

    return *this;
}


bool
OsduAnyCrsGeoJsonGeometryElementStruct::isValid() const
{
    return !type.empty() && CoordinatesValid(coordinates_point, coordinates_line, coordinates_polygon, coordinates_multipolygon);
}


std::vector<std::string>
OsduAnyCrsGeoJsonGeometryElementStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduAnyCrsGeoJsonGeometryElementStruct::isValid() failed.");

    if (type.empty()) {
        result.emplace_back("OsduAnyCrsGeoJsonGeometryElementStruct::type must have a value.");
    }
    if (!CoordinatesValid(coordinates_point, coordinates_line, coordinates_polygon, coordinates_multipolygon)) {
        result.emplace_back("OsduAnyCrsGeoJsonGeometryElementStruct::coordinates must have valid values matching the type.");
    }
    return result;
}

} // namespace segysdk::metadata::osdu
