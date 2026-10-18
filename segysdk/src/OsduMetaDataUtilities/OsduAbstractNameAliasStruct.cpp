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

#include "../OsduMetaDataUtilities/OsduAbstractNameAliasStruct.h"


namespace segysdk::metadata::osdu
{

OsduAbstractNameAliasStruct::OsduAbstractNameAliasStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractNameAliasStruct::OsduAbstractNameAliasStruct(const OsduAbstractNameAliasStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    aliasName(other.aliasName),
    aliasNameTypeID(other.aliasNameTypeID),
    definitionOrganisationID(other.definitionOrganisationID),
    effectiveDateTime(other.effectiveDateTime),
    terminationDateTime(other.terminationDateTime)
{
    /* Empty */
}

OsduAbstractNameAliasStruct::OsduAbstractNameAliasStruct(OsduAbstractNameAliasStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    aliasName(std::move(other.aliasName)),
    aliasNameTypeID(std::move(other.aliasNameTypeID)),
    definitionOrganisationID(std::move(other.definitionOrganisationID)),
    effectiveDateTime(std::move(other.effectiveDateTime)),
    terminationDateTime(std::move(other.terminationDateTime))
{
    /* Empty */
}

OsduAbstractNameAliasStruct&
OsduAbstractNameAliasStruct::operator=(const OsduAbstractNameAliasStruct& rhs)
{
    return *this = OsduAbstractNameAliasStruct(rhs);
}

OsduAbstractNameAliasStruct&
OsduAbstractNameAliasStruct::operator=(OsduAbstractNameAliasStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    aliasName = std::move(rhs.aliasName);
    aliasNameTypeID = std::move(rhs.aliasNameTypeID);
    definitionOrganisationID = std::move(rhs.definitionOrganisationID);
    effectiveDateTime = std::move(rhs.effectiveDateTime);
    terminationDateTime = std::move(rhs.terminationDateTime);

    return *this;
}

bool
OsduAbstractNameAliasStruct::isValid() const
{
    return aliasNameTypeIDValid() and definitionOrganisationIDValid();
}

bool
OsduAbstractNameAliasStruct::aliasNameTypeIDValid() const
{
    return !aliasNameTypeID.has_value() or isValidOSDUReferenceIdString(aliasNameTypeID.value());
}

bool
OsduAbstractNameAliasStruct::definitionOrganisationIDValid() const
{
    return !definitionOrganisationID.has_value() or isValidOSDUReferenceIdString(definitionOrganisationID.value());
}

std::vector<std::string>
OsduAbstractNameAliasStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> result{"OsduAbstractNameAliasesStruct:isValid() failed"};
    if (!aliasNameTypeIDValid()) {
        result.emplace_back("OsduAbstractNameAliasesStruct:aliasNameTypeIDValid() failed");
    }

    if (!definitionOrganisationIDValid()) {
        result.emplace_back("OsduAbstractNameAliasesStruct:definitionOrganisationIDValid() failed");
    }

    return result;
}

} // namespace segysdk::metadata::osdu
