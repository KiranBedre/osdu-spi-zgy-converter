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

#include "../OsduMetaDataUtilities/OsduLineageAssertionStruct.h"

namespace segysdk::metadata::osdu
{
OsduLineageAssertionStruct::OsduLineageAssertionStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduLineageAssertionStruct::OsduLineageAssertionStruct(const OsduLineageAssertionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    id(other.id),
    lineageRelationshipType(other.lineageRelationshipType)
{
    /* Empty */
}

OsduLineageAssertionStruct::OsduLineageAssertionStruct(OsduLineageAssertionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    id(std::move(other.id)),
    lineageRelationshipType(std::move(other.lineageRelationshipType))
{
    /* Empty */
}

OsduLineageAssertionStruct&
OsduLineageAssertionStruct::operator=(const OsduLineageAssertionStruct& rhs)
{
    return *this = OsduLineageAssertionStruct(rhs);
}

OsduLineageAssertionStruct&
OsduLineageAssertionStruct::operator=(OsduLineageAssertionStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    id = std::move(rhs.id);
    lineageRelationshipType = std::move(rhs.lineageRelationshipType);

    return *this;
}

bool
OsduLineageAssertionStruct::idValid() const
{

        return !id.has_value() or isValidOSDUReferenceIdString(id.value());
}

bool
OsduLineageAssertionStruct::lineageRelationshipTypeValid() const
{
        return !lineageRelationshipType.has_value() or isValidOSDUReferenceIdString(lineageRelationshipType.value());
}

bool
OsduLineageAssertionStruct::isValid() const
{
    return idValid() && lineageRelationshipTypeValid();
}

std::vector<std::string>
OsduLineageAssertionStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> result{"OsduLineageAssertionStruct::isValid() failed."};
    if (!idValid()) {
        result.emplace_back("OsduLineageAssertionStruct::idValid() failed.");
    }

    if (!lineageRelationshipTypeValid()) {
        result.emplace_back("OsduLineageAssertionStruct::lineageRelationshipTypeValid() failed.");
    }

    return result;
}

} // namespace segysdk::metadata::osdu
