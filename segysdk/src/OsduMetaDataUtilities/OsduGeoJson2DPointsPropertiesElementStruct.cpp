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

#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElementStruct.h"

#include <algorithm>
#include <utility>

namespace segysdk::metadata::osdu
{
 OsduGeoJson2DPointsPropertiesElementStruct:: OsduGeoJson2DPointsPropertiesElementStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

 OsduGeoJson2DPointsPropertiesElementStruct:: OsduGeoJson2DPointsPropertiesElementStruct(const  OsduGeoJson2DPointsPropertiesElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    shotpointNumbers(other.shotpointNumbers),
    commonMidPointNumbers((other.commonMidPointNumbers))
{
    /* Empty */
}

 OsduGeoJson2DPointsPropertiesElementStruct:: OsduGeoJson2DPointsPropertiesElementStruct( OsduGeoJson2DPointsPropertiesElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    shotpointNumbers(std::move(other.shotpointNumbers)),
    commonMidPointNumbers(std::move(other.commonMidPointNumbers))
{
    /* Empty */
}

 OsduGeoJson2DPointsPropertiesElementStruct&
 OsduGeoJson2DPointsPropertiesElementStruct::operator=(const  OsduGeoJson2DPointsPropertiesElementStruct& rhs)
{
    return *this =  OsduGeoJson2DPointsPropertiesElementStruct(rhs);
}

 OsduGeoJson2DPointsPropertiesElementStruct&
 OsduGeoJson2DPointsPropertiesElementStruct::operator=( OsduGeoJson2DPointsPropertiesElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    shotpointNumbers = std::move(rhs.shotpointNumbers);
    commonMidPointNumbers = std::move(rhs.commonMidPointNumbers);
    return *this;
}

bool
 OsduGeoJson2DPointsPropertiesElementStruct::isValid() const
{
    return !shotpointNumbers.empty() or !commonMidPointNumbers.empty();
}

std::vector<std::string>
 OsduGeoJson2DPointsPropertiesElementStruct::performAttributeValidityScan() const
{
        if (isValid()) {
            return {};
        }

    std::vector<std::string> result;
    result.emplace_back("OsduGeoJson2DPointsPropertiesElementStruct::isValid() failed.");

    return result;
}

} // namespace segysdk::metadata::osdu
