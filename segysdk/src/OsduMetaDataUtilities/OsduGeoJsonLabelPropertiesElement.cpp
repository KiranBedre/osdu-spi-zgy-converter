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

#include "../OsduMetaDataUtilities/OsduGeoJsonLabelPropertiesElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureProperties.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesLiterals.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonLabelPropertiesElementStruct.h"

namespace segysdk::metadata::osdu
{
OsduGeoJsonLabelPropertiesElement::OsduGeoJsonLabelPropertiesElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), featurePropertyStruct_(OsduGeoJsonLabelPropertiesElementStruct(osduPartition)) {}

OsduGeoJsonLabelPropertiesElement::OsduGeoJsonLabelPropertiesElement(const OsduGeoJsonLabelPropertiesElement& other)
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(other.featurePropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonLabelPropertiesElement::OsduGeoJsonLabelPropertiesElement(OsduGeoJsonLabelPropertiesElement&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(std::move(other.featurePropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonLabelPropertiesElement&
OsduGeoJsonLabelPropertiesElement::operator=(const OsduGeoJsonLabelPropertiesElement& rhs)
{
    return *this = OsduGeoJsonLabelPropertiesElement(rhs);
}

OsduGeoJsonLabelPropertiesElement&
OsduGeoJsonLabelPropertiesElement::operator=(OsduGeoJsonLabelPropertiesElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    featurePropertyStruct_ = std::move(rhs.featurePropertyStruct_);

    return *this;
}

OsduGeoJsonLabelPropertiesElement::OsduGeoJsonLabelPropertiesElement(OsduGeoJsonLabelPropertiesElementStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), featurePropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonLabelPropertiesElement::OsduGeoJsonLabelPropertiesElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), featurePropertyStruct_(OsduGeoJsonLabelPropertiesElementStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduGeoJsonLabelPropertiesElement::isValidInput() const
{
    return featurePropertyStruct_.isValid();
}

void
OsduGeoJsonLabelPropertiesElement::initializeJson()
{
    clearJsonObject();
}

void
OsduGeoJsonLabelPropertiesElement::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(featurePropertyStruct_.attributeName, featurePropertyStruct_.stringValue);
    setOptionalParameter(featurePropertyStruct_.attributeName, featurePropertyStruct_.boolValue);
    setOptionalParameter(featurePropertyStruct_.attributeName, featurePropertyStruct_.intValue);
    setOptionalParameter(featurePropertyStruct_.attributeName, featurePropertyStruct_.doubleValue);
}

void
OsduGeoJsonLabelPropertiesElement::initializeMetadataStruct()
{
    featurePropertyStruct_ = OsduGeoJsonLabelPropertiesElementStruct(getOsduPartition());
}

void
OsduGeoJsonLabelPropertiesElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    const auto memberList = getJsonMemberNames();
    for (const auto& member : memberList) {
        if (member == OsduGeoJsonFeaturePropertiesLiterals::ijNumbers or 
            member == OsduGeoJsonFeaturePropertiesLiterals::PointProperties  or
            member == OsduGeoJsonFeaturePropertiesLiterals::Inline or 
            member == OsduGeoJsonFeaturePropertiesLiterals::Crossline or 
            member == OsduGeoJsonFeaturePropertiesLiterals::FeatureProperties or 
            member == OsduGeoJsonFeaturePropertiesLiterals::PointsProperties
        ) 
        {
            continue;
        }

        featurePropertyStruct_.attributeName = member;
        if (getJson()[member].isString()) {
            featurePropertyStruct_.stringValue = getJson()[member].asString();
        }
        if (getJson()[member].isBool()) {
            featurePropertyStruct_.boolValue = getJson()[member].asBool();
        }
        if (getJson()[member].isInt64()) {
            featurePropertyStruct_.intValue = getJson()[member].asLargestInt();
        }
        if (getJson()[member].isInt()) {
            featurePropertyStruct_.intValue = getJson()[member].asInt();
        }
        if (getJson()[member].isDouble()) {
            featurePropertyStruct_.intValue = getJson()[member].asDouble();
        }
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonLabelPropertiesElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonLabelPropertiesElementStruct>(featurePropertyStruct_);
}

} // namespace segysdk::metadata::osdu
