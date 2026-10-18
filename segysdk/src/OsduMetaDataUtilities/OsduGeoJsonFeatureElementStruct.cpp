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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureElementStruct.h"


namespace segysdk::metadata::osdu
{
OsduGeoJsonFeatureElementStruct::OsduGeoJsonFeatureElementStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition), 
    properties(OsduGeoJsonFeaturePropertiesStruct(osduPartition)), 
    geometry(OsduGeoJsonGeometryElementStruct(osduPartition))
{
    /* Empty */
}

OsduGeoJsonFeatureElementStruct::OsduGeoJsonFeatureElementStruct(const OsduGeoJsonFeatureElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    properties(other.properties),
    geometry(other.geometry),
    bbox(other.bbox)
{
    /* Empty */
}

OsduGeoJsonFeatureElementStruct::OsduGeoJsonFeatureElementStruct(OsduGeoJsonFeatureElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    properties(std::move(other.properties)),
    geometry(std::move(other.geometry)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduGeoJsonFeatureElementStruct&
OsduGeoJsonFeatureElementStruct::operator=(const OsduGeoJsonFeatureElementStruct& rhs)
{
    return *this = OsduGeoJsonFeatureElementStruct(rhs);
}

OsduGeoJsonFeatureElementStruct&
OsduGeoJsonFeatureElementStruct::operator=(OsduGeoJsonFeatureElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    type = std::move(rhs.type);
    properties = std::move(rhs.properties);
    geometry = std::move(rhs.geometry);
    bbox = std::move(rhs.bbox);

    return *this;
}

bool
OsduGeoJsonFeatureElementStruct::isValid() const
{
    return !type.empty();
}

std::vector<std::string>
OsduGeoJsonFeatureElementStruct::performAttributeValidityScan() const
{
    std::vector<std::string> result;
    if (!isValid()) {
        result.emplace_back("OsduGeoJsonFeatureElementStruct::isValid() failed.");
    }

    if (type.empty()) {
        result.emplace_back("OsduGeoJsonFeatureElementStruct: type cannot be empty.");
    }

    return result;
}

void
OsduGeoJsonFeatureElementStruct::propagatePartition(const std::string& partition)
{
    geometry.setPartition(partition);
    properties.setPartition(partition);
}

} // namespace segysdk::metadata::osdu
