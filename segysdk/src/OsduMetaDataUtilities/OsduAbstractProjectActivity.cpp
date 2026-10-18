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

#include "../OsduMetaDataUtilities/OsduAbstractProjectActivity.h"

#include "../OsduMetaDataUtilities/OsduAbstractActivityParameter.h"
#include "../OsduMetaDataUtilities/OsduAbstractActivityState.h"

#include "../OsduMetaDataUtilities/OsduAbstractProjectActivityLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractProjectActivity::OsduAbstractProjectActivity(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    projectActivityStruct_(osduPartition)
{
    /* Empty */
}
OsduAbstractProjectActivity::OsduAbstractProjectActivity(OsduAbstractProjectActivityStruct activityStruct)
  : OsduMetadataBase(activityStruct.getPartition()), projectActivityStruct_(std::move(activityStruct))
{
    populateJsonObject();
}

OsduAbstractProjectActivity::OsduAbstractProjectActivity(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), projectActivityStruct_(OsduAbstractProjectActivityStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractProjectActivity::isValidInput() const
{
    return projectActivityStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractProjectActivity::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractProjectActivityStruct>(projectActivityStruct_);
}

void
OsduAbstractProjectActivity::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractProjectActivity::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractProjectActivityLiterals::ActivityTemplateID, projectActivityStruct_.templateId);
    setOptionalParameter(OsduAbstractProjectActivityLiterals::ParentProjectID, projectActivityStruct_.parentProjectId);
    setOptionalArrayStructParameter<OsduAbstractActivityParameter>(OsduAbstractProjectActivityLiterals::Parameters, projectActivityStruct_.parameters);
    setOptionalArrayStructParameter<OsduAbstractActivityState>(OsduAbstractProjectActivityLiterals::ActivityStates, projectActivityStruct_.states);
    setOptionalStructParameter<OsduAbstractActivityState>(OsduAbstractProjectActivityLiterals::LastActivityState, projectActivityStruct_.lastState);
}

void
OsduAbstractProjectActivity::initializeMetadataStruct()
{
    projectActivityStruct_ = OsduAbstractProjectActivityStruct(getOsduPartition());
}

void
OsduAbstractProjectActivity::populateMetadataStruct()
{
    initializeMetadataStruct();

    projectActivityStruct_.templateId = getOptionalParameter<std::string>(OsduAbstractProjectActivityLiterals::ActivityTemplateID);
    projectActivityStruct_.parentProjectId = getOptionalParameter<std::string>(OsduAbstractProjectActivityLiterals::ParentProjectID);
    projectActivityStruct_.parameters = getOptionalArrayStructParameter<OsduAbstractActivityParameter, OsduAbstractActivityParameterStruct>(OsduAbstractProjectActivityLiterals::Parameters);
    projectActivityStruct_.states = getOptionalArrayStructParameter<OsduAbstractActivityState, OsduAbstractActivityStateStruct>(OsduAbstractProjectActivityLiterals::ActivityStates);
    projectActivityStruct_.lastState = getOptionalStructParameter<OsduAbstractActivityState, OsduAbstractActivityStateStruct>(OsduAbstractProjectActivityLiterals::LastActivityState);
}
} // namespace segysdk::metadata::osdu
