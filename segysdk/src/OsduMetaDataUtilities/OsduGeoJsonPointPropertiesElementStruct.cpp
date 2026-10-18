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

#include "../OsduMetaDataUtilities/OsduGeoJsonPointPropertiesElementStruct.h"

#include <algorithm>
#include <utility>

namespace segysdk::metadata::osdu
{
OsduGeoJsonPointPropertiesElementStruct::OsduGeoJsonPointPropertiesElementStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition)), 
    inlineNo(0), crosslineNo(0)
{
    /* Empty */
}

OsduGeoJsonPointPropertiesElementStruct::OsduGeoJsonPointPropertiesElementStruct(const OsduGeoJsonPointPropertiesElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    inlineNo(other.inlineNo),
    crosslineNo(other.crosslineNo),
    label(other.label)
{
    /* Empty */
}

OsduGeoJsonPointPropertiesElementStruct::OsduGeoJsonPointPropertiesElementStruct(OsduGeoJsonPointPropertiesElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    inlineNo(other.inlineNo),
    crosslineNo(other.crosslineNo),
    label(std::move(other.label))
{
    /* Empty */
}

OsduGeoJsonPointPropertiesElementStruct&
OsduGeoJsonPointPropertiesElementStruct::operator=(const OsduGeoJsonPointPropertiesElementStruct& rhs)
{
    return *this = OsduGeoJsonPointPropertiesElementStruct(rhs);
}

OsduGeoJsonPointPropertiesElementStruct&
OsduGeoJsonPointPropertiesElementStruct::operator=(OsduGeoJsonPointPropertiesElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    inlineNo = rhs.inlineNo;
    crosslineNo = rhs.crosslineNo;
    label = std::move(rhs.label);

    return *this;
}

bool
OsduGeoJsonPointPropertiesElementStruct::isValid() const
{
    return !label.has_value() or !label.value().empty();
}

std::vector<std::string>
OsduGeoJsonPointPropertiesElementStruct::performAttributeValidityScan() const
{
        if (isValid()) {
            return {};
        }

    std::vector<std::string> result;
    result.emplace_back("OsduGeoJsonPointPropertiesElementStruct::isValid() failed.");

    return result;
}

} // namespace segysdk::metadata::osdu
