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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesElementStruct.h"

#include <algorithm>
#include <utility>

namespace segysdk::metadata::osdu
{
OsduGeoJsonFeaturePropertiesElementStruct::OsduGeoJsonFeaturePropertiesElementStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesElementStruct::OsduGeoJsonFeaturePropertiesElementStruct(const OsduGeoJsonFeaturePropertiesElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    lineName(other.lineName),
    pointTypes(other.pointTypes)
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesElementStruct::OsduGeoJsonFeaturePropertiesElementStruct(OsduGeoJsonFeaturePropertiesElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    lineName(other.lineName),
    pointTypes(std::move(other.pointTypes))
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesElementStruct&
OsduGeoJsonFeaturePropertiesElementStruct::operator=(const OsduGeoJsonFeaturePropertiesElementStruct& rhs)
{
    return *this = OsduGeoJsonFeaturePropertiesElementStruct(rhs);
}

OsduGeoJsonFeaturePropertiesElementStruct&
OsduGeoJsonFeaturePropertiesElementStruct::operator=(OsduGeoJsonFeaturePropertiesElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    lineName = std::move(rhs.lineName);
    pointTypes = std::move(rhs.pointTypes);
    return *this;
}

bool
OsduGeoJsonFeaturePropertiesElementStruct::isValid() const
{
    return !lineName.empty() and !pointTypes.empty();
}

std::vector<std::string>
OsduGeoJsonFeaturePropertiesElementStruct::performAttributeValidityScan() const
{
        if (isValid()) {
            return {};
        }

    std::vector<std::string> result;
    result.emplace_back("OsduGeoJsonFeaturePropertiesElementStruct::isValid() failed.");
    if (lineName.empty()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesElementStruct::LineName cannot be empty.");
    }
    if (pointTypes.empty()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesElementStruct::PointTypes cannot be empty.");
    }
    return result;
}

} // namespace segysdk::metadata::osdu
