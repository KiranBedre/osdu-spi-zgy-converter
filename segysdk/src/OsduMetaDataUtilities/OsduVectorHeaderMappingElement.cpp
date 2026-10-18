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

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingElement.h"

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingElementStruct.h"
#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingLiterals.h"

namespace segysdk::metadata::osdu
{
OsduVectorHeaderMappingElement::OsduVectorHeaderMappingElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), vhmElementStruct_(OsduVectorHeaderMappingElementStruct(osduPartition))
{
    /* Empty */
}

OsduVectorHeaderMappingElement::OsduVectorHeaderMappingElement(OsduVectorHeaderMappingElementStruct vhmStruct)
    : OsduMetadataBase(vhmStruct.getPartition()), vhmElementStruct_(std::move(vhmStruct))
{
    populateJsonObject();
}

OsduVectorHeaderMappingElement::OsduVectorHeaderMappingElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), vhmElementStruct_(OsduVectorHeaderMappingElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduVectorHeaderMappingElement::isValidInput() const
{
    return vhmElementStruct_.isValid();
}

void
OsduVectorHeaderMappingElement::initializeJson()
{
    removeMember(OsduVectorHeaderMappingLiterals::KeyName);
    removeMember(OsduVectorHeaderMappingLiterals::Position);
    removeMember(OsduVectorHeaderMappingLiterals::WordFormat);
    removeMember(OsduVectorHeaderMappingLiterals::WordWidth);
    removeMember(OsduVectorHeaderMappingLiterals::ScalarIndicator);
    removeMember(OsduVectorHeaderMappingLiterals::ScalarOverride);
    removeMember(OsduVectorHeaderMappingLiterals::UoM);
}

void
OsduVectorHeaderMappingElement::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduVectorHeaderMappingLiterals::KeyName, vhmElementStruct_.keyName);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::Position, vhmElementStruct_.position);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::WordFormat, vhmElementStruct_.wordFormatType);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::WordWidth, vhmElementStruct_.wordWidth);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::ScalarIndicator, vhmElementStruct_.scalarIndicator);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::ScalarOverride, vhmElementStruct_.scalarOverride);
    setOptionalParameter(OsduVectorHeaderMappingLiterals::UoM, vhmElementStruct_.uom);
}

void
OsduVectorHeaderMappingElement::initializeMetadataStruct()
{
    vhmElementStruct_ = OsduVectorHeaderMappingElementStruct(getOsduPartition());
}

void
OsduVectorHeaderMappingElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    vhmElementStruct_.keyName = getOptionalParameter<std::string>(OsduVectorHeaderMappingLiterals::KeyName);
    vhmElementStruct_.position = getOptionalParameter<int64_t>(OsduVectorHeaderMappingLiterals::Position);
    vhmElementStruct_.wordFormatType = getOptionalParameter<std::string>(OsduVectorHeaderMappingLiterals::WordFormat);
    vhmElementStruct_.wordWidth = getOptionalParameter<int64_t>(OsduVectorHeaderMappingLiterals::WordWidth);
    vhmElementStruct_.scalarIndicator = getOptionalParameter<std::string>(OsduVectorHeaderMappingLiterals::ScalarIndicator);
    vhmElementStruct_.scalarOverride = getOptionalParameter<double>(OsduVectorHeaderMappingLiterals::ScalarOverride);
    vhmElementStruct_.uom = getOptionalParameter<std::string>(OsduVectorHeaderMappingLiterals::UoM);
}

std::shared_ptr<OsduMetadataStructBase>
OsduVectorHeaderMappingElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduVectorHeaderMappingElementStruct>(vhmElementStruct_);
}

} // namespace segysdk::metadata::osdu
