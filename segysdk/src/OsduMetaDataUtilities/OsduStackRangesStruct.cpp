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

#include "../OsduMetaDataUtilities/OsduStackRangesStruct.h"


namespace segysdk::metadata::osdu
{


OsduStackRangesStruct::OsduStackRangesStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduStackRangesStruct::OsduStackRangesStruct(const OsduStackRangesStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    minimum(other.minimum),
    maximum(other.maximum)
{
    /* Empty */
}

OsduStackRangesStruct::OsduStackRangesStruct(OsduStackRangesStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    minimum(other.minimum),
    maximum(other.maximum)
{
    /* Empty */
}

OsduStackRangesStruct&
OsduStackRangesStruct::operator=(const OsduStackRangesStruct& rhs)
{
    return *this = OsduStackRangesStruct(rhs);
}

OsduStackRangesStruct&
OsduStackRangesStruct::operator=(OsduStackRangesStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    minimum = rhs.minimum;
    maximum = rhs.maximum;

    return *this;
}

bool
OsduStackRangesStruct::isValid() const
{
    return (!minimum.has_value() and !maximum.has_value()) or (minimum.has_value() and maximum.has_value());
}

std::vector<std::string>
OsduStackRangesStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    return {"OsduStackRangesStruct:isValid() failed",
            "OsduStackRangesStruct:Cannot have only one of minimum and maximum defined."};
}

} // namespace segysdk::metadata::osdu
