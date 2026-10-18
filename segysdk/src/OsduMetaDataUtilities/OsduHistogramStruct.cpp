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

#include "../OsduMetaDataUtilities/OsduHistogramStruct.h"


namespace segysdk::metadata::osdu
{
OsduHistogramStruct::OsduHistogramStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduHistogramStruct::OsduHistogramStruct(const OsduHistogramStruct& other)
    : OsduMetadataStructBase(other.getPartition()),
    name(other.name),
    description(other.description),
    bucketMin(other.bucketMin),
    bucketMax(other.bucketMax),
    bucketSize(other.bucketSize),
    bucketCount(other.bucketCount),
    buckets(other.buckets)
{
    /* Empty */
}

OsduHistogramStruct::OsduHistogramStruct(OsduHistogramStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    name(std::move(other.name)),
    description(std::move(other.description)),
    bucketMin(other.bucketMin),
    bucketMax(other.bucketMax),
    bucketSize(other.bucketSize),
    bucketCount(other.bucketCount),
    buckets(std::move(other.buckets))
{
    /* Empty */
}

OsduHistogramStruct&
OsduHistogramStruct::operator=(const OsduHistogramStruct& rhs)
{
    return *this = OsduHistogramStruct(rhs);
}

OsduHistogramStruct&
OsduHistogramStruct::operator=(OsduHistogramStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    name = std::move(rhs.name);
    description = std::move(rhs.description);
    bucketMin = rhs.bucketMin;
    bucketMax = rhs.bucketMax;
    bucketSize = rhs.bucketSize;
    bucketCount = rhs.bucketCount;
    buckets = std::move(rhs.buckets);

    return *this;
}

bool
OsduHistogramStruct::isValid() const
{
    // All parameters are optional
    return true;
}

std::vector<std::string>
OsduHistogramStruct::performAttributeValidityScan() const
{
    if (!isValid()) {
        return std::vector<std::string>{"OsduHistogramStruct::isValid() failed."};
    }
    return {};
}

} // namespace segysdk::metadata::osdu
