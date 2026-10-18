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

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryTypeStruct.h"


namespace segysdk::metadata::osdu
{
OsduGeoJsonGeometryTypeStruct::OsduGeoJsonGeometryTypeStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGeoJsonGeometryTypeStruct::OsduGeoJsonGeometryTypeStruct(const OsduGeoJsonGeometryTypeStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    coordinates(other.coordinates),
    bbox(other.bbox)
{
    /* Empty */
}


OsduGeoJsonGeometryTypeStruct::OsduGeoJsonGeometryTypeStruct(OsduGeoJsonGeometryTypeStruct&& other) noexcept
    : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    coordinates(std::move(other.coordinates)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduGeoJsonGeometryTypeStruct&
OsduGeoJsonGeometryTypeStruct::operator=(const OsduGeoJsonGeometryTypeStruct& rhs)
{
    return *this = OsduGeoJsonGeometryTypeStruct(rhs);
}

OsduGeoJsonGeometryTypeStruct&
OsduGeoJsonGeometryTypeStruct::operator=(OsduGeoJsonGeometryTypeStruct&& rhs) noexcept
{
    type = std::move(rhs.type);
    coordinates = std::move(rhs.coordinates);
    bbox = std::move(rhs.bbox);

    setPartition(rhs.getPartition());

    return *this;
}


bool
OsduGeoJsonGeometryTypeStruct::isValid() const
{
    return !type.empty() and !coordinates.empty();
}

std::vector<std::string>
OsduGeoJsonGeometryTypeStruct::performAttributeValidityScan() const
{
    std::vector<std::string> result;
    if (!isValid()) {
        result.emplace_back("OsduGeoJsonGeometryTypeStruct::isValid() failed.");
    }

    if (type.empty()) {
        result.emplace_back("OsduGeoJsonGeometryTypeStruct:: type should not be an empty string.");
    }
    if (coordinates.empty()) {
        result.emplace_back("OsduGeoJsonGeometryTypeStruct:: coordinates should not be an empty array.");
    }
    return result;
}

} // namespace segysdk::metadata::osdu
