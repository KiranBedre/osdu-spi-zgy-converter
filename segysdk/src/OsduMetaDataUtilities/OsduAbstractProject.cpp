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

#include "../OsduMetaDataUtilities/OsduAbstractProject.h"

#include "../OsduMetaDataUtilities/OsduFundsAuthorization.h"
#include "../OsduMetaDataUtilities/OsduContractor.h"
#include "../OsduMetaDataUtilities/OsduPersonal.h"
#include "../OsduMetaDataUtilities/OsduProjectSpecification.h"
#include "../OsduMetaDataUtilities/OsduProjectState.h"

#include "../OsduMetaDataUtilities/OsduAbstractProjectLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractProject::OsduAbstractProject(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    projectStruct_(osduPartition)
{
    /* Empty */
}
OsduAbstractProject::OsduAbstractProject(OsduAbstractProjectStruct projectStruct)
  : OsduMetadataBase(projectStruct.getPartition()), projectStruct_(std::move(projectStruct))
{
    populateJsonObject();
}

OsduAbstractProject::OsduAbstractProject(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), projectStruct_(OsduAbstractProjectStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractProject::isValidInput() const
{
    return projectStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractProject::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractProjectStruct>(projectStruct_);
}

void
OsduAbstractProject::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractProject::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractProjectLiterals::ProjectID, projectStruct_.id);
    setOptionalParameter(OsduAbstractProjectLiterals::ProjectName, projectStruct_.name);
    setOptionalParameter(OsduAbstractProjectLiterals::Purpose, projectStruct_.purpose);
    setOptionalParameter(OsduAbstractProjectLiterals::ProjectBeginDate, projectStruct_.beginDate);
    setOptionalParameter(OsduAbstractProjectLiterals::ProjectEndDate, projectStruct_.endDate);
    setOptionalArrayStructParameter<OsduFundsAuthorization>(OsduAbstractProjectLiterals::FundsAuthorizations, projectStruct_.fundsAuthorizations);
    setOptionalArrayParameter(OsduAbstractProjectLiterals::ContractIDs, projectStruct_.contractIds);
    setOptionalParameter(OsduAbstractProjectLiterals::Operator, projectStruct_.operator_);
    setOptionalArrayStructParameter<OsduContractor>(OsduAbstractProjectLiterals::Contractors, projectStruct_.contractorsStruct);
    setOptionalArrayStructParameter<OsduPersonal>(OsduAbstractProjectLiterals::Personnel, projectStruct_.personnelStruct);
    setOptionalArrayStructParameter<OsduProjectSpecification>(OsduAbstractProjectLiterals::ProjectSpecifications, projectStruct_.specificationsStruct);
    setOptionalArrayStructParameter<OsduProjectState>(OsduAbstractProjectLiterals::ProjectStates, projectStruct_.statesStruct);
}

void
OsduAbstractProject::initializeMetadataStruct()
{
    projectStruct_ = OsduAbstractProjectStruct(getOsduPartition());
}

void
OsduAbstractProject::populateMetadataStruct()
{
    initializeMetadataStruct();

    projectStruct_.id = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::ProjectID);
    projectStruct_.name = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::ProjectName);
    projectStruct_.purpose = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::Purpose);
    projectStruct_.beginDate = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::ProjectBeginDate);
    projectStruct_.endDate = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::ProjectEndDate);
    projectStruct_.fundsAuthorizations = getOptionalArrayStructParameter<OsduFundsAuthorization, OsduFundsAuthorizationStruct>(OsduAbstractProjectLiterals::FundsAuthorizations);
    projectStruct_.contractIds = getOptionalArrayParameter<std::string>(OsduAbstractProjectLiterals::ContractIDs);
    projectStruct_.operator_ = getOptionalParameter<std::string>(OsduAbstractProjectLiterals::Operator);
    projectStruct_.contractorsStruct = getOptionalArrayStructParameter<OsduContractor, OsduContractorStruct>(OsduAbstractProjectLiterals::Contractors);
    projectStruct_.personnelStruct = getOptionalArrayStructParameter<OsduPersonal, OsduPersonalStruct>(OsduAbstractProjectLiterals::Personnel);
    projectStruct_.specificationsStruct = getOptionalArrayStructParameter<OsduProjectSpecification, OsduProjectSpecificationStruct>(OsduAbstractProjectLiterals::ProjectSpecifications);
    projectStruct_.statesStruct = getOptionalArrayStructParameter<OsduProjectState, OsduProjectStateStruct>(OsduAbstractProjectLiterals::ProjectStates);
}
} // namespace segysdk::metadata::osdu
