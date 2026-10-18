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

#include "../OsduMetaDataUtilities/OsduProjectSpecification.h"

#include "../OsduMetaDataUtilities/OsduProjectSpecificationLiterals.h"

namespace segysdk::metadata::osdu
{
OsduProjectSpecification::OsduProjectSpecification(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    projectSpecification_(osduPartition)
{
    /* Empty */
}
OsduProjectSpecification::OsduProjectSpecification(OsduProjectSpecificationStruct projectSpecification)
  : OsduMetadataBase(projectSpecification.getPartition()), projectSpecification_(std::move(projectSpecification))
{
    populateJsonObject();
}

OsduProjectSpecification::OsduProjectSpecification(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), projectSpecification_(OsduProjectSpecificationStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduProjectSpecification::isValidInput() const
{
    return projectSpecification_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduProjectSpecification::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduProjectSpecificationStruct>(projectSpecification_);
}

void
OsduProjectSpecification::initializeJson()
{
    clearJsonObject();
}

void
OsduProjectSpecification::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduProjectSpecificationLiterals::EffectiveDateTime, projectSpecification_.effectiveDateTime);
    setOptionalParameter(OsduProjectSpecificationLiterals::TerminationDateTime, projectSpecification_.terminationDateTime);
    setOptionalParameter(OsduProjectSpecificationLiterals::ProjectSpecificationQuantity, projectSpecification_.quantity);
    setOptionalParameter(OsduProjectSpecificationLiterals::ProjectSpecificationDateTime, projectSpecification_.dateTime);
    setOptionalParameter(OsduProjectSpecificationLiterals::ProjectSpecificationIndicator, projectSpecification_.indicator);
    setOptionalParameter(OsduProjectSpecificationLiterals::ProjectSpecificationText, projectSpecification_.text);
    setOptionalParameter(OsduProjectSpecificationLiterals::UnitOfMeasureID, projectSpecification_.unitOfMeasureID);
    setOptionalParameter(OsduProjectSpecificationLiterals::ParameterTypeID, projectSpecification_.parameterTypeID);
}

void
OsduProjectSpecification::initializeMetadataStruct()
{
    projectSpecification_ = OsduProjectSpecificationStruct(getOsduPartition());
}

void
OsduProjectSpecification::populateMetadataStruct()
{
    initializeMetadataStruct();

    projectSpecification_.effectiveDateTime = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::EffectiveDateTime);
    projectSpecification_.terminationDateTime = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::TerminationDateTime);
    projectSpecification_.quantity = getOptionalParameter<double>(OsduProjectSpecificationLiterals::ProjectSpecificationQuantity);
    projectSpecification_.dateTime = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::ProjectSpecificationDateTime);
    projectSpecification_.indicator = getOptionalParameter<bool>(OsduProjectSpecificationLiterals::ProjectSpecificationIndicator);
    projectSpecification_.text = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::ProjectSpecificationText);
    projectSpecification_.unitOfMeasureID = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::UnitOfMeasureID);
    projectSpecification_.parameterTypeID = getOptionalParameter<std::string>(OsduProjectSpecificationLiterals::ParameterTypeID);
}
} // namespace segysdk::metadata::osdu
