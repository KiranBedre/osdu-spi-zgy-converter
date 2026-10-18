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

#include "../OsduMetaDataUtilities/OsduMetaArrayStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduMetaArrayStruct::OsduMetaArrayStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduMetaArrayStruct::OsduMetaArrayStruct(const OsduMetaArrayStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    metaStructs(other.metaStructs)
{
    /* Empty */
}

OsduMetaArrayStruct::OsduMetaArrayStruct(OsduMetaArrayStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    metaStructs(std::move(other.metaStructs))
{
    /* Empty */
}

OsduMetaArrayStruct&
OsduMetaArrayStruct::operator=(const OsduMetaArrayStruct& rhs)
{
    return *this = OsduMetaArrayStruct(rhs);
}

OsduMetaArrayStruct&
OsduMetaArrayStruct::operator=(OsduMetaArrayStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    metaStructs = std::move(rhs.metaStructs);

    return *this;
}

OsduMetaArrayStruct::OsduMetaArrayStruct(std::string osduPartition, const std::vector<OsduMetaElementStruct>& meta_structs)
    : OsduMetadataStructBase(std::move(osduPartition)), metaStructs(meta_structs)
{
    /* Empty */
}

bool
OsduMetaArrayStruct::allStructsOK() const
{
    return std::all_of(metaStructs.cbegin(), metaStructs.cend(), [](const OsduMetaElementStruct& x) { return x.isValid(); });
}

bool
OsduMetaArrayStruct::isValid() const
{
    return  metaStructs.empty() or allStructsOK();
}

std::vector<std::string>
OsduMetaArrayStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results{"OsduMetaArrayStruct::isValid() failed."};
    performArrayStructValidityScan(results, metaStructs);

    return results;
}

void
OsduMetaArrayStruct::propagatePartition(const std::string& partition)
{
    for (auto& element : metaStructs) {
        element.setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
