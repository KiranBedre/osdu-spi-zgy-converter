/*
Copyright 2024 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduPersonal.h"

#include "../OsduMetaDataUtilities/OsduPersonalLiterals.h"

namespace segysdk::metadata::osdu
{
OsduPersonal::OsduPersonal(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    personalStruct_(osduPartition)
{
    /* Empty */
}
OsduPersonal::OsduPersonal(OsduPersonalStruct personalStruct)
  : OsduMetadataBase(personalStruct.getPartition()), personalStruct_(std::move(personalStruct))
{
    populateJsonObject();
}

OsduPersonal::OsduPersonal(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), personalStruct_(OsduPersonalStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduPersonal::isValidInput() const
{
    return personalStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduPersonal::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduPersonalStruct>(personalStruct_);
}

void
OsduPersonal::initializeJson()
{
    clearJsonObject();
}

void
OsduPersonal::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduPersonalLiterals::PersonName, personalStruct_.personName);
    setOptionalParameter(OsduPersonalLiterals::CompanyOrganisationID, personalStruct_.companyOrganisationID);
    setOptionalParameter(OsduPersonalLiterals::ProjectRoleID, personalStruct_.projectRoleID);
}

void
OsduPersonal::initializeMetadataStruct()
{
    personalStruct_ = OsduPersonalStruct(getOsduPartition());
}

void
OsduPersonal::populateMetadataStruct()
{
    initializeMetadataStruct();

    personalStruct_.personName = getOptionalParameter<std::string>(OsduPersonalLiterals::PersonName);
    personalStruct_.companyOrganisationID = getOptionalParameter<std::string>(OsduPersonalLiterals::CompanyOrganisationID);
    personalStruct_.projectRoleID = getOptionalParameter<std::string>(OsduPersonalLiterals::ProjectRoleID);
}
} // namespace segysdk::metadata::osdu
