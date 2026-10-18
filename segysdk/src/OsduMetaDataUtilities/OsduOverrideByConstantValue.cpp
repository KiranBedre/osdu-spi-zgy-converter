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

#include "../OsduMetaDataUtilities/OsduOverrideByConstantValue.h"

#include "../OsduMetaDataUtilities/OsduOverrideByConstantValueLiterals.h"

namespace segysdk::metadata::osdu
{
OsduOverrideByConstantValue::OsduOverrideByConstantValue(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), vhmCvoStruct_(OsduOverrideByConstantValueStruct(osduPartition))
{
    /* Empty */
}

OsduOverrideByConstantValue::OsduOverrideByConstantValue(OsduOverrideByConstantValueStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), vhmCvoStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduOverrideByConstantValue::OsduOverrideByConstantValue(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), vhmCvoStruct_(OsduOverrideByConstantValueStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduOverrideByConstantValue::isValidInput() const
{
    return vhmCvoStruct_.isValid();
}

void
OsduOverrideByConstantValue::initializeJson()
{
    removeMember(OsduOverrideByConstantValueLiterals::KeyName);
    removeMember(OsduOverrideByConstantValueLiterals::OverrideByConstant);
}

void
OsduOverrideByConstantValue::populateJsonObject()
{
    initializeJson();

    setParameter(OsduOverrideByConstantValueLiterals::KeyName, vhmCvoStruct_.keyName);
    setParameter(OsduOverrideByConstantValueLiterals::OverrideByConstant, vhmCvoStruct_.overrideByConstant);
}

void
OsduOverrideByConstantValue::initializeMetadataStruct()
{
    vhmCvoStruct_ = OsduOverrideByConstantValueStruct(getOsduPartition());
}

void
OsduOverrideByConstantValue::populateMetadataStruct()
{
    initializeMetadataStruct();

    vhmCvoStruct_.keyName = getParameter<std::string>(OsduOverrideByConstantValueLiterals::KeyName);
    vhmCvoStruct_.overrideByConstant = getParameter<double>(OsduOverrideByConstantValueLiterals::OverrideByConstant);
}

std::shared_ptr<OsduMetadataStructBase>
OsduOverrideByConstantValue::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduOverrideByConstantValueStruct>(vhmCvoStruct_);
}
} // namespace segysdk::metadata::osdu
