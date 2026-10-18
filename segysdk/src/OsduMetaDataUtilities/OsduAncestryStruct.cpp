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

#include "../OsduMetaDataUtilities/OsduAncestryStruct.h"

namespace segysdk::metadata::osdu
{
OsduAncestryStruct::OsduAncestryStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAncestryStruct::OsduAncestryStruct(std::string osduPartition, const std::vector<std::string>& ancestors)
    : OsduMetadataStructBase(std::move(osduPartition)), 
    parents(ancestors)
{
    /* Empty */
}

OsduAncestryStruct::OsduAncestryStruct(const OsduAncestryStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    parents(other.parents)
{
    /* Empty */
}

OsduAncestryStruct::OsduAncestryStruct(OsduAncestryStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    parents(std::move(other.parents))
{
    /* Empty */
}

OsduAncestryStruct&
OsduAncestryStruct::operator=(const OsduAncestryStruct& rhs)
{
    return *this = OsduAncestryStruct(rhs);
}

OsduAncestryStruct&
OsduAncestryStruct::operator=(OsduAncestryStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    parents = std::move(rhs.parents);

    return *this;
}

bool
OsduAncestryStruct::isValid() const
{
    return isValidIdStringArray(parents, IdStringCheckType::Reference);
}

std::vector<std::string>
OsduAncestryStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduAncestryStruct::isValid() failed.");

    performIdStringArrayValidityScan(results, parents, "OsduAncestryStruct::parents", IdStringCheckType::Reference);

    return results;
}
} // namespace segysdk::metadata::osdu
