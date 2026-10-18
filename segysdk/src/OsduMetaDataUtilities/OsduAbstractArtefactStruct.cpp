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

#include "../OsduMetaDataUtilities/OsduAbstractArtefactStruct.h"


namespace segysdk::metadata::osdu
{

OsduAbstractArtefactStruct::OsduAbstractArtefactStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractArtefactStruct::OsduAbstractArtefactStruct(const OsduAbstractArtefactStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    roleID(other.roleID),
    resourceKind(other.resourceKind),
    resourceID(other.resourceID)
{
    /* Empty */
}

OsduAbstractArtefactStruct::OsduAbstractArtefactStruct(OsduAbstractArtefactStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    roleID(std::move(other.roleID)),
    resourceKind(std::move(other.resourceKind)),
    resourceID(std::move(other.resourceID))
{
    /* Empty */
}

OsduAbstractArtefactStruct&
OsduAbstractArtefactStruct::operator=(const OsduAbstractArtefactStruct& rhs)
{
    return *this = OsduAbstractArtefactStruct(rhs);
}

OsduAbstractArtefactStruct&
OsduAbstractArtefactStruct::operator=(OsduAbstractArtefactStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    roleID = std::move(rhs.roleID);
    resourceKind = std::move(rhs.resourceKind);
    resourceID = std::move(rhs.resourceID);

    return *this;
}

bool
OsduAbstractArtefactStruct::isValid() const
{
    return roleIDValid() and resourceIdValid() and ResourceKindValid();
}

bool
OsduAbstractArtefactStruct::roleIDValid() const
{
    return !roleID.has_value() or isValidOSDUReferenceIdString(roleID.value());
}

bool
OsduAbstractArtefactStruct::resourceIdValid() const
{
    return !resourceID.has_value() or isValidOSDUReferenceIdString(resourceID.value());
}

bool
OsduAbstractArtefactStruct::ResourceKindValid() const
{
    return !resourceKind.has_value() or !resourceKind.value().empty();
}

std::vector<std::string>
OsduAbstractArtefactStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> result{
        "OsduAbstractArtefactStruct:isValid() failed"};

    if (!roleIDValid()) {
        result.emplace_back("OsduAbstractArtefactStruct::roleIDValid() failed ");
    }
    if (!resourceIdValid()) {
        result.emplace_back("OsduAbstractArtefactStruct::resourceIdValid() failed ");
    }
    if (!ResourceKindValid()) {
        result.emplace_back("OsduAbstractArtefactStruct::ResourceKindValid() failed ");
    }

    return result;
}

} // namespace segysdk::metadata::osdu
