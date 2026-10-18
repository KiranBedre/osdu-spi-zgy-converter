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

#include "../OsduMetaDataUtilities/OsduAbstractReviewerStruct.h"


namespace segysdk::metadata::osdu
{


OsduAbstractReviewerStruct::OsduAbstractReviewerStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractReviewerStruct::OsduAbstractReviewerStruct(const OsduAbstractReviewerStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    emailAddress(other.emailAddress),
    phoneNumber(other.phoneNumber),
    roleTypeID(other.roleTypeID),
    dataGovernanceRoleTypeID(other.dataGovernanceRoleTypeID),
    workflowPersonaTypeID(other.workflowPersonaTypeID),
    comment(other.comment),
    organisationID(other.organisationID),
    name(other.name)
{
    /* Empty */
}

OsduAbstractReviewerStruct::OsduAbstractReviewerStruct(OsduAbstractReviewerStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    emailAddress(std::move(other.emailAddress)),
    phoneNumber(std::move(other.phoneNumber)),
    roleTypeID(std::move(other.roleTypeID)),
    dataGovernanceRoleTypeID(std::move(other.dataGovernanceRoleTypeID)),
    workflowPersonaTypeID(std::move(other.workflowPersonaTypeID)),
    comment(std::move(other.comment)),
    organisationID(std::move(other.organisationID)),
    name(std::move(other.name))
{
    /* Empty */
}

OsduAbstractReviewerStruct&
OsduAbstractReviewerStruct::operator=(const OsduAbstractReviewerStruct& rhs)
{
    return *this = OsduAbstractReviewerStruct(rhs);
}

OsduAbstractReviewerStruct&
OsduAbstractReviewerStruct::operator=(OsduAbstractReviewerStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    emailAddress = std::move(rhs.emailAddress);
    phoneNumber = std::move(rhs.phoneNumber);
    roleTypeID = std::move(rhs.roleTypeID);
    dataGovernanceRoleTypeID = std::move(rhs.dataGovernanceRoleTypeID);
    workflowPersonaTypeID = std::move(rhs.workflowPersonaTypeID);
    comment = std::move(rhs.comment);
    organisationID = std::move(rhs.organisationID);
    name = std::move(rhs.name);
    return *this;
}

bool
OsduAbstractReviewerStruct::isValid() const
{
    return roleTypeIDValid() and dataGovernanceRoleTypeIDValid() and workflowPersonaTypeIDValid() and organisationIDValid();
}

bool
OsduAbstractReviewerStruct::roleTypeIDValid() const
{
    return !roleTypeID.has_value() or isValidOSDUReferenceIdString(roleTypeID.value());
}

bool
OsduAbstractReviewerStruct::dataGovernanceRoleTypeIDValid() const
{
    return !dataGovernanceRoleTypeID.has_value() or isValidOSDUReferenceIdString(dataGovernanceRoleTypeID.value());
}

bool
OsduAbstractReviewerStruct::workflowPersonaTypeIDValid() const
{
    return !workflowPersonaTypeID.has_value() or isValidOSDUReferenceIdString(workflowPersonaTypeID.value());
}

bool
OsduAbstractReviewerStruct::organisationIDValid() const
{
    return !organisationID.has_value() or isValidOSDUReferenceIdString(organisationID.value());
}


std::vector<std::string>
OsduAbstractReviewerStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> result{
        "OsduAbstractReviewerStruct:isValid() failed"};

    if (!roleTypeIDValid()) {
        result.emplace_back("OsduAbstractReviewerStruct:roleTypeIDValid() failed");
    }

    if (!dataGovernanceRoleTypeIDValid()) {
        result.emplace_back("OsduAbstractReviewerStruct:dataGovernanceRoleTypeIDValid() failed");
    }
    if (!workflowPersonaTypeIDValid()) {
        result.emplace_back("OsduAbstractReviewerStruct:workflowPersonaTypeIDValid() failed");
    }
    if (!organisationIDValid()) {
        result.emplace_back("OsduAbstractReviewerStruct:organisationIDValid() failed");
    }
    return result;
}

} // namespace segysdk::metadata::osdu
