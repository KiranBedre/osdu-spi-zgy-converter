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

#include "../OsduMetaDataUtilities/OsduGeoJsonPointPropertiesElement.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonPointPropertiesElementStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonPointPropertiesElementLiterals.h"

namespace segysdk::metadata::osdu
{

OsduGeoJsonPointPropertiesElement::OsduGeoJsonPointPropertiesElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), pointPropertyStruct_(OsduGeoJsonPointPropertiesElementStruct(osduPartition)) {}

OsduGeoJsonPointPropertiesElement::OsduGeoJsonPointPropertiesElement(const OsduGeoJsonPointPropertiesElement& other)
  : OsduMetadataBase(other.getOsduPartition()), pointPropertyStruct_(other.pointPropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonPointPropertiesElement::OsduGeoJsonPointPropertiesElement(OsduGeoJsonPointPropertiesElement&& other) noexcept
  : OsduMetadataBase(other.getOsduPartition()), pointPropertyStruct_(std::move(other.pointPropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonPointPropertiesElement&
OsduGeoJsonPointPropertiesElement::operator=(const OsduGeoJsonPointPropertiesElement& rhs)
{
    return *this = OsduGeoJsonPointPropertiesElement(rhs);
}

OsduGeoJsonPointPropertiesElement&
OsduGeoJsonPointPropertiesElement::operator=(OsduGeoJsonPointPropertiesElement&& rhs) noexcept
{
    setJson(rhs.getJson());

    pointPropertyStruct_ = std::move(rhs.pointPropertyStruct_);

    return *this;
}

OsduGeoJsonPointPropertiesElement::OsduGeoJsonPointPropertiesElement(OsduGeoJsonPointPropertiesElementStruct constantValueOverrideStruct)
  : OsduMetadataBase(constantValueOverrideStruct.getPartition()), pointPropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonPointPropertiesElement::OsduGeoJsonPointPropertiesElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), pointPropertyStruct_(OsduGeoJsonPointPropertiesElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGeoJsonPointPropertiesElement::isValidInput() const
{
    return pointPropertyStruct_.isValid();
}

void
OsduGeoJsonPointPropertiesElement::initializeJson()
{
    removeMember(OsduGeoJsonPointPropertiesElementLiterals::Inline);
    removeMember(OsduGeoJsonPointPropertiesElementLiterals::Crossline);
    removeMember(OsduGeoJsonPointPropertiesElementLiterals::Label);
}

void
OsduGeoJsonPointPropertiesElement::populateJsonObject()
{
    clearJsonObject();

    setParameter(OsduGeoJsonPointPropertiesElementLiterals::Inline, pointPropertyStruct_.inlineNo);
    setParameter(OsduGeoJsonPointPropertiesElementLiterals::Crossline, pointPropertyStruct_.crosslineNo);
    setOptionalParameter(OsduGeoJsonPointPropertiesElementLiterals::Label, pointPropertyStruct_.label);
}

void
OsduGeoJsonPointPropertiesElement::initializeMetadataStruct()
{
    pointPropertyStruct_ = OsduGeoJsonPointPropertiesElementStruct(getOsduPartition());
}

void
OsduGeoJsonPointPropertiesElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    pointPropertyStruct_.inlineNo = getParameter<int>(OsduGeoJsonPointPropertiesElementLiterals::Inline);
    pointPropertyStruct_.crosslineNo = getParameter<int>(OsduGeoJsonPointPropertiesElementLiterals::Crossline);

    pointPropertyStruct_.label = getOptionalParameter<std::string>(OsduGeoJsonPointPropertiesElementLiterals::Label);

}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonPointPropertiesElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonPointPropertiesElementStruct>(pointPropertyStruct_);
}

} // namespace segysdk::metadata::osdu
