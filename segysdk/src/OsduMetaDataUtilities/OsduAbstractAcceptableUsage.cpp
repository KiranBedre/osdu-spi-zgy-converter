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

#include "../OsduMetaDataUtilities/OsduAbstractAcceptableUsage.h"

#include "../OsduMetaDataUtilities/OsduAbstractAcceptableUsageLiterals.h"

namespace segysdk::metadata::osdu
{

OsduAbstractAcceptableUsage::OsduAbstractAcceptableUsage(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), acceptableUsageStruct_(OsduAbstractAcceptableUsageStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractAcceptableUsage::OsduAbstractAcceptableUsage(const OsduAbstractAcceptableUsage& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      acceptableUsageStruct_(other.acceptableUsageStruct_)
{
   /* Empty */
}

OsduAbstractAcceptableUsage::OsduAbstractAcceptableUsage(OsduAbstractAcceptableUsage&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      acceptableUsageStruct_(std::move(other.acceptableUsageStruct_))
{
    /* Empty */
}

OsduAbstractAcceptableUsage&
OsduAbstractAcceptableUsage::operator=(const OsduAbstractAcceptableUsage& rhs)
{
    return *this = OsduAbstractAcceptableUsage(rhs);
}

OsduAbstractAcceptableUsage&
OsduAbstractAcceptableUsage::operator=(OsduAbstractAcceptableUsage&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    acceptableUsageStruct_ = std::move(rhs.acceptableUsageStruct_);

    return *this;
}

OsduAbstractAcceptableUsage::OsduAbstractAcceptableUsage(OsduAbstractAcceptableUsageStruct acceptableUsageStruct)
    : OsduMetadataBase(acceptableUsageStruct.getPartition()), acceptableUsageStruct_(std::move(acceptableUsageStruct))
{
    populateJsonObject();
}

OsduAbstractAcceptableUsage::OsduAbstractAcceptableUsage(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), acceptableUsageStruct_(OsduAbstractAcceptableUsageStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractAcceptableUsage::isValidInput() const
{
    return acceptableUsageStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractAcceptableUsage::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractAcceptableUsageStruct>(acceptableUsageStruct_);
}

void
OsduAbstractAcceptableUsage::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractAcceptableUsage::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractAcceptableUsageLiterals::WorkflowUsageTypeID, acceptableUsageStruct_.workflowUsageTypeID);
    setOptionalParameter(OsduAbstractAcceptableUsageLiterals::WorkflowPersonaTypeID, acceptableUsageStruct_.workflowPersonaTypeID);
    setOptionalParameter(OsduAbstractAcceptableUsageLiterals::DataQualityID, acceptableUsageStruct_.dataQualityID);
    setOptionalParameter(OsduAbstractAcceptableUsageLiterals::DataQualityRuleSetID, acceptableUsageStruct_.dataQualityRuleSetID);
    setOptionalParameter(OsduAbstractAcceptableUsageLiterals::ValueChainStatusTypeID, acceptableUsageStruct_.valueChainStatusTypeID);
}

void
OsduAbstractAcceptableUsage::initializeMetadataStruct() {
    acceptableUsageStruct_ = OsduAbstractAcceptableUsageStruct(getOsduPartition());
}

void
OsduAbstractAcceptableUsage::populateMetadataStruct()
{
    acceptableUsageStruct_.workflowUsageTypeID = getOptionalParameter<std::string>(OsduAbstractAcceptableUsageLiterals::WorkflowUsageTypeID);
    acceptableUsageStruct_.workflowPersonaTypeID = getOptionalParameter<std::string>(OsduAbstractAcceptableUsageLiterals::WorkflowPersonaTypeID);
    acceptableUsageStruct_.dataQualityID = getOptionalParameter<std::string>(OsduAbstractAcceptableUsageLiterals::DataQualityID);
    acceptableUsageStruct_.dataQualityRuleSetID = getOptionalParameter<std::string>(OsduAbstractAcceptableUsageLiterals::DataQualityRuleSetID);
    acceptableUsageStruct_.valueChainStatusTypeID = getOptionalParameter<std::string>(OsduAbstractAcceptableUsageLiterals::ValueChainStatusTypeID);
}

} // namespace segysdk::metadata::osdu
