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

#include "../OsduMetaDataUtilities/OsduProjectState.h"

#include "../OsduMetaDataUtilities/OsduProjectStateLiterals.h"

namespace segysdk::metadata::osdu
{
OsduProjectState::OsduProjectState(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    projectState_(osduPartition)
{
    /* Empty */
}
OsduProjectState::OsduProjectState(OsduProjectStateStruct projectState)
  : OsduMetadataBase(projectState.getPartition()), projectState_(std::move(projectState))
{
    populateJsonObject();
}

OsduProjectState::OsduProjectState(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), projectState_(OsduProjectStateStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduProjectState::isValidInput() const
{
    return projectState_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduProjectState::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduProjectStateStruct>(projectState_);
}

void
OsduProjectState::initializeJson()
{
    clearJsonObject();
}

void
OsduProjectState::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduProjectStateLiterals::EffectiveDateTime, projectState_.effectiveDateTime);
    setOptionalParameter(OsduProjectStateLiterals::TerminationDateTime, projectState_.terminationDateTime);
    setOptionalParameter(OsduProjectStateLiterals::ProjectStateTypeID, projectState_.projectStateTypeID);
}

void
OsduProjectState::initializeMetadataStruct()
{
    projectState_ = OsduProjectStateStruct(getOsduPartition());
}

void
OsduProjectState::populateMetadataStruct()
{
    initializeMetadataStruct();

    projectState_.effectiveDateTime = getOptionalParameter<std::string>(OsduProjectStateLiterals::EffectiveDateTime);
    projectState_.terminationDateTime = getOptionalParameter<std::string>(OsduProjectStateLiterals::TerminationDateTime);
    projectState_.projectStateTypeID = getOptionalParameter<std::string>(OsduProjectStateLiterals::ProjectStateTypeID);
}
} // namespace segysdk::metadata::osdu
