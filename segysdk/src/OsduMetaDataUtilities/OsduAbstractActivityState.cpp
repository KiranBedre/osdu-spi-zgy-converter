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

#include "../OsduMetaDataUtilities/OsduAbstractActivityState.h"

#include "../OsduMetaDataUtilities/OsduAbstractActivityStateLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractActivityState::OsduAbstractActivityState(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    activityStateStruct_(osduPartition)
{
    /* Empty */
}
OsduAbstractActivityState::OsduAbstractActivityState(OsduAbstractActivityStateStruct activityStateStruct)
  : OsduMetadataBase(activityStateStruct.getPartition()), activityStateStruct_(std::move(activityStateStruct))
{
    populateJsonObject();
}

OsduAbstractActivityState::OsduAbstractActivityState(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), activityStateStruct_(OsduAbstractActivityStateStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractActivityState::isValidInput() const
{
    return activityStateStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractActivityState::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractActivityStateStruct>(activityStateStruct_);
}

void
OsduAbstractActivityState::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractActivityState::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractActivityStateLiterals::EffectiveDateTime, activityStateStruct_.effectiveDateTime);
    setOptionalParameter(OsduAbstractActivityStateLiterals::TerminationDateTime, activityStateStruct_.terminationDateTime);
    setOptionalParameter(OsduAbstractActivityStateLiterals::ActivityStatusID, activityStateStruct_.statusId);
    setOptionalParameter(OsduAbstractActivityStateLiterals::Remark, activityStateStruct_.remark);
}

void
OsduAbstractActivityState::initializeMetadataStruct()
{
    activityStateStruct_ = OsduAbstractActivityStateStruct(getOsduPartition());
}

void
OsduAbstractActivityState::populateMetadataStruct()
{
    initializeMetadataStruct();

    activityStateStruct_.effectiveDateTime = getOptionalParameter<std::string>(OsduAbstractActivityStateLiterals::EffectiveDateTime);
    activityStateStruct_.terminationDateTime = getOptionalParameter<std::string>(OsduAbstractActivityStateLiterals::TerminationDateTime);
    activityStateStruct_.statusId = getOptionalParameter<std::string>(OsduAbstractActivityStateLiterals::ActivityStatusID);
    activityStateStruct_.remark = getOptionalParameter<std::string>(OsduAbstractActivityStateLiterals::Remark);
}
} // namespace segysdk::metadata::osdu
