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

#include "../OsduMetaDataUtilities/OsduTimeLapseStruct.h"


namespace segysdk::metadata::osdu
{


OsduTimeLapseStruct::OsduTimeLapseStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduTimeLapseStruct::OsduTimeLapseStruct(const OsduTimeLapseStruct& other)
    : OsduMetadataStructBase(other.getPartition()),
    timeSeriesID(other.timeSeriesID),
    timeIndex(other.timeIndex)
{
    /* Empty */
}

OsduTimeLapseStruct::OsduTimeLapseStruct(OsduTimeLapseStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    timeSeriesID(std::move(other.timeSeriesID)),
    timeIndex(other.timeIndex)
{
    /* Empty */
}

OsduTimeLapseStruct&
OsduTimeLapseStruct::operator=(const OsduTimeLapseStruct& rhs)
{
    return *this = OsduTimeLapseStruct(rhs);
}

OsduTimeLapseStruct&
OsduTimeLapseStruct::operator=(OsduTimeLapseStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    timeSeriesID = std::move(rhs.timeSeriesID);
    timeIndex = rhs.timeIndex;

    return *this;
}

bool
OsduTimeLapseStruct::isValid() const
{
    return !timeSeriesID.has_value() or isValidOSDUReferenceIdString(timeSeriesID.value());
}

std::vector<std::string>
OsduTimeLapseStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    return {
        "OsduTimeLapseStruct: timeSeriesID does not have a valid id value"};
}

} // namespace segysdk::metadata::osdu
