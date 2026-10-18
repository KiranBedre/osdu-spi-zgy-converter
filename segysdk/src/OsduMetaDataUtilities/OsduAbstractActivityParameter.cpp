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

#include "../OsduMetaDataUtilities/OsduAbstractActivityParameter.h"

#include "../OsduMetaDataUtilities/OsduParameterKey.h"

#include "../OsduMetaDataUtilities/OsduAbstractActivityParameterLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractActivityParameter::OsduAbstractActivityParameter(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    activityParameterStruct_(osduPartition)
{
    /* Empty */
}
OsduAbstractActivityParameter::OsduAbstractActivityParameter(OsduAbstractActivityParameterStruct activityParameterStruct)
  : OsduMetadataBase(activityParameterStruct.getPartition()), activityParameterStruct_(std::move(activityParameterStruct))
{
    populateJsonObject();
}

OsduAbstractActivityParameter::OsduAbstractActivityParameter(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), activityParameterStruct_(OsduAbstractActivityParameterStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractActivityParameter::isValidInput() const
{
    return activityParameterStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractActivityParameter::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractActivityParameterStruct>(activityParameterStruct_);
}

void
OsduAbstractActivityParameter::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractActivityParameter::populateJsonObject()
{
    initializeJson();

    setParameter(OsduAbstractActivityParameterLiterals::Title, activityParameterStruct_.title);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::Index, activityParameterStruct_.index);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::Selection, activityParameterStruct_.selection);
    setOptionalArrayStructParameter<OsduParameterKey>(OsduAbstractActivityParameterLiterals::Keys, activityParameterStruct_.keysStruct);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::DataObjectParameter, activityParameterStruct_.dataObject);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::DataQuantityParameter, activityParameterStruct_.dataQuantity);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::IntegerQuantityParameter, activityParameterStruct_.integerQuantity);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::StringParameter, activityParameterStruct_.string);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::TimeIndexParameter, activityParameterStruct_.timeIndex);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::BooleanParameter, activityParameterStruct_.boolean);
    setParameter(OsduAbstractActivityParameterLiterals::ParameterKindID, activityParameterStruct_.kindId);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::ParameterRoleID, activityParameterStruct_.roleId);
    setOptionalParameter(OsduAbstractActivityParameterLiterals::DataQuantityParameterUOMID, activityParameterStruct_.dataQuantityUomId);
}

void
OsduAbstractActivityParameter::initializeMetadataStruct()
{
    activityParameterStruct_ = OsduAbstractActivityParameterStruct(getOsduPartition());
}

void
OsduAbstractActivityParameter::populateMetadataStruct()
{
    initializeMetadataStruct();

    activityParameterStruct_.title = getParameter<std::string>(OsduAbstractActivityParameterLiterals::Title);
    activityParameterStruct_.index = getOptionalParameter<int64_t>(OsduAbstractActivityParameterLiterals::Index);
    activityParameterStruct_.selection = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::Selection);
    activityParameterStruct_.keysStruct = getOptionalArrayStructParameter<OsduParameterKey, OsduParameterKeyStruct>(OsduAbstractActivityParameterLiterals::Keys);
    activityParameterStruct_.dataObject = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::DataObjectParameter);
    activityParameterStruct_.dataQuantity = getOptionalParameter<double>(OsduAbstractActivityParameterLiterals::DataQuantityParameter);
    activityParameterStruct_.integerQuantity = getOptionalParameter<int64_t>(OsduAbstractActivityParameterLiterals::IntegerQuantityParameter);
    activityParameterStruct_.string = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::StringParameter);
    activityParameterStruct_.timeIndex = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::TimeIndexParameter);
    activityParameterStruct_.boolean = getOptionalParameter<bool>(OsduAbstractActivityParameterLiterals::BooleanParameter);
    activityParameterStruct_.kindId = getParameter<std::string>(OsduAbstractActivityParameterLiterals::ParameterKindID);
    activityParameterStruct_.roleId = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::ParameterRoleID);
    activityParameterStruct_.dataQuantityUomId = getOptionalParameter<std::string>(OsduAbstractActivityParameterLiterals::DataQuantityParameterUOMID);
}
} // namespace segysdk::metadata::osdu
