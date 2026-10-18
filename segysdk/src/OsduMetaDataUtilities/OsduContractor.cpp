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

#include "../OsduMetaDataUtilities/OsduContractor.h"

#include "../OsduMetaDataUtilities/OsduContractorLiterals.h"

namespace segysdk::metadata::osdu
{
OsduContractor::OsduContractor(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    contractorStruct_(osduPartition)
{
    /* Empty */
}
OsduContractor::OsduContractor(OsduContractorStruct contractorStruct)
  : OsduMetadataBase(contractorStruct.getPartition()), contractorStruct_(std::move(contractorStruct))
{
    populateJsonObject();
}

OsduContractor::OsduContractor(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), contractorStruct_(OsduContractorStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduContractor::isValidInput() const
{
    return contractorStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduContractor::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduContractorStruct>(contractorStruct_);
}

void
OsduContractor::initializeJson()
{
    clearJsonObject();
}

void
OsduContractor::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduContractorLiterals::ContractorOrganisationID, contractorStruct_.organisationID);
    setOptionalParameter(OsduContractorLiterals::ContractorCrew, contractorStruct_.crew);
    setOptionalParameter(OsduContractorLiterals::ContractorTypeID, contractorStruct_.typeID);
}

void
OsduContractor::initializeMetadataStruct()
{
    contractorStruct_ = OsduContractorStruct(getOsduPartition());
}

void
OsduContractor::populateMetadataStruct()
{
    initializeMetadataStruct();

    contractorStruct_.organisationID = getOptionalParameter<std::string>(OsduContractorLiterals::ContractorOrganisationID);
    contractorStruct_.crew = getOptionalParameter<std::string>(OsduContractorLiterals::ContractorCrew);
    contractorStruct_.typeID = getOptionalParameter<std::string>(OsduContractorLiterals::ContractorTypeID);
}
} // namespace segysdk::metadata::osdu
