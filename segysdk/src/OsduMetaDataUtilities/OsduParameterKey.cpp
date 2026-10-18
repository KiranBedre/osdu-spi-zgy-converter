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

#include "../OsduMetaDataUtilities/OsduParameterKey.h"

#include "../OsduMetaDataUtilities/OsduParameterKeyLiterals.h"

namespace segysdk::metadata::osdu
{
OsduParameterKey::OsduParameterKey(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    parameterKeyStruct_(osduPartition)
{
    /* Empty */
}
OsduParameterKey::OsduParameterKey(OsduParameterKeyStruct parameterKeyStruct)
  : OsduMetadataBase(parameterKeyStruct.getPartition()), parameterKeyStruct_(std::move(parameterKeyStruct))
{
    populateJsonObject();
}

OsduParameterKey::OsduParameterKey(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), parameterKeyStruct_(OsduParameterKeyStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduParameterKey::isValidInput() const
{
    return parameterKeyStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduParameterKey::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduParameterKeyStruct>(parameterKeyStruct_);
}

void
OsduParameterKey::initializeJson()
{
    clearJsonObject();
}

void
OsduParameterKey::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduParameterKeyLiterals::ObjectParameterKey, parameterKeyStruct_.object);
    setOptionalParameter(OsduParameterKeyLiterals::TimeIndexParameterKey, parameterKeyStruct_.timeIndex);
    setOptionalParameter(OsduParameterKeyLiterals::ParameterKey, parameterKeyStruct_.parameter);
    setOptionalParameter(OsduParameterKeyLiterals::IntegerParameterKey, parameterKeyStruct_.integer);
    setOptionalParameter(OsduParameterKeyLiterals::StringParameterKey, parameterKeyStruct_.string);
}

void
OsduParameterKey::initializeMetadataStruct()
{
    parameterKeyStruct_ = OsduParameterKeyStruct(getOsduPartition());
}

void
OsduParameterKey::populateMetadataStruct()
{
    initializeMetadataStruct();

    parameterKeyStruct_.object = getOptionalParameter<std::string>(OsduParameterKeyLiterals::ObjectParameterKey);
    parameterKeyStruct_.timeIndex = getOptionalParameter<std::string>(OsduParameterKeyLiterals::TimeIndexParameterKey);
    parameterKeyStruct_.parameter = getOptionalParameter<std::string>(OsduParameterKeyLiterals::ParameterKey);
    parameterKeyStruct_.integer = getOptionalParameter<int64_t>(OsduParameterKeyLiterals::IntegerParameterKey);
    parameterKeyStruct_.string = getOptionalParameter<std::string>(OsduParameterKeyLiterals::StringParameterKey);
}
} // namespace segysdk::metadata::osdu
