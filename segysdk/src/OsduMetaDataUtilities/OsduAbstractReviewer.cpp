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

#include <utility>

#include "../OsduMetaDataUtilities/OsduAbstractReviewer.h"

#include "../OsduMetaDataUtilities/OsduAbstractReviewerLiterals.h"

namespace segysdk::metadata::osdu
{

OsduAbstractReviewer::OsduAbstractReviewer(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), abstractReviewerStruct_(OsduAbstractReviewerStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractReviewer::OsduAbstractReviewer(const OsduAbstractReviewer& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractReviewerStruct_(other.abstractReviewerStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractReviewer::OsduAbstractReviewer(OsduAbstractReviewer&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractReviewerStruct_(std::move(other.abstractReviewerStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractReviewer&
OsduAbstractReviewer::operator=(const OsduAbstractReviewer& rhs)
{
    return *this = OsduAbstractReviewer(rhs);
}

OsduAbstractReviewer&
OsduAbstractReviewer::operator=(OsduAbstractReviewer&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    abstractReviewerStruct_ = std::move(rhs.abstractReviewerStruct_);

    return *this;
}

OsduAbstractReviewer::OsduAbstractReviewer(OsduAbstractReviewerStruct tagElementStruct)
    : OsduMetadataBase(tagElementStruct.getPartition()), abstractReviewerStruct_(std::move(tagElementStruct))
{
    populateJsonObject();
}

OsduAbstractReviewer::OsduAbstractReviewer(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), abstractReviewerStruct_(OsduAbstractReviewerStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractReviewer::isValidInput() const
{
    return abstractReviewerStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractReviewer::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractReviewerStruct>(abstractReviewerStruct_);
}

void
OsduAbstractReviewer::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractReviewer::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractReviewerLiterals::EmailAddress, abstractReviewerStruct_.emailAddress);
    setOptionalParameter(OsduAbstractReviewerLiterals::PhoneNumber, abstractReviewerStruct_.phoneNumber);
    setOptionalParameter(OsduAbstractReviewerLiterals::RoleTypeID, abstractReviewerStruct_.roleTypeID);
    setOptionalParameter(OsduAbstractReviewerLiterals::DataGovernanceRoleTypeID, abstractReviewerStruct_.dataGovernanceRoleTypeID);
    setOptionalParameter(OsduAbstractReviewerLiterals::WorkflowPersonaTypeID, abstractReviewerStruct_.workflowPersonaTypeID);
    setOptionalParameter(OsduAbstractReviewerLiterals::Comment, abstractReviewerStruct_.comment);
    setOptionalParameter(OsduAbstractReviewerLiterals::OrganisationID, abstractReviewerStruct_.organisationID);
    setOptionalParameter(OsduAbstractReviewerLiterals::Name, abstractReviewerStruct_.name);
}

void
OsduAbstractReviewer::initializeMetadataStruct() {
    abstractReviewerStruct_ = OsduAbstractReviewerStruct(getOsduPartition());
}

void
OsduAbstractReviewer::populateMetadataStruct()
{
    abstractReviewerStruct_.emailAddress = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::EmailAddress);
    abstractReviewerStruct_.phoneNumber = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::PhoneNumber);
    abstractReviewerStruct_.roleTypeID = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::RoleTypeID);
    abstractReviewerStruct_.dataGovernanceRoleTypeID = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::DataGovernanceRoleTypeID);
    abstractReviewerStruct_.workflowPersonaTypeID = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::WorkflowPersonaTypeID);
    abstractReviewerStruct_.comment = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::Comment);
    abstractReviewerStruct_.organisationID = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::OrganisationID);
    abstractReviewerStruct_.name = getOptionalParameter<std::string>(OsduAbstractReviewerLiterals::Name);
}

} // namespace segysdk::metadata::osdu
