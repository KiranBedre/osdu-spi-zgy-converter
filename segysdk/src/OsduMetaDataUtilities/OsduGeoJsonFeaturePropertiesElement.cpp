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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesElement.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesElementStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesElementLiterals.h"

namespace segysdk::metadata::osdu
{

OsduGeoJsonFeaturePropertiesElement::OsduGeoJsonFeaturePropertiesElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), featurePropertyStruct_(OsduGeoJsonFeaturePropertiesElementStruct(osduPartition)) {}

OsduGeoJsonFeaturePropertiesElement::OsduGeoJsonFeaturePropertiesElement(const OsduGeoJsonFeaturePropertiesElement& other)
  : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(other.featurePropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeaturePropertiesElement::OsduGeoJsonFeaturePropertiesElement(OsduGeoJsonFeaturePropertiesElement&& other) noexcept
  : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(std::move(other.featurePropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeaturePropertiesElement&
OsduGeoJsonFeaturePropertiesElement::operator=(const OsduGeoJsonFeaturePropertiesElement& rhs)
{
    return *this = OsduGeoJsonFeaturePropertiesElement(rhs);
}

OsduGeoJsonFeaturePropertiesElement&
OsduGeoJsonFeaturePropertiesElement::operator=(OsduGeoJsonFeaturePropertiesElement&& rhs) noexcept
{
    setJson(rhs.getJson());

    featurePropertyStruct_ = std::move(rhs.featurePropertyStruct_);

    return *this;
}

OsduGeoJsonFeaturePropertiesElement::OsduGeoJsonFeaturePropertiesElement(OsduGeoJsonFeaturePropertiesElementStruct constantValueOverrideStruct)
  : OsduMetadataBase(constantValueOverrideStruct.getPartition()), featurePropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonFeaturePropertiesElement::OsduGeoJsonFeaturePropertiesElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), featurePropertyStruct_(OsduGeoJsonFeaturePropertiesElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGeoJsonFeaturePropertiesElement::isValidInput() const
{
    return featurePropertyStruct_.isValid();
}

void
OsduGeoJsonFeaturePropertiesElement::initializeJson()
{
    removeMember(OsduGeoJsonFeaturePropertiesElementLiterals::LineName);
    removeMember(OsduGeoJsonFeaturePropertiesElementLiterals::PointTypes);
}

void
OsduGeoJsonFeaturePropertiesElement::populateJsonObject()
{
    initializeJson();

    setParameter(OsduGeoJsonFeaturePropertiesElementLiterals::LineName, featurePropertyStruct_.lineName);
    InitializeJsonArray(OsduGeoJsonFeaturePropertiesElementLiterals::PointTypes);
    for (const auto& pointType : featurePropertyStruct_.pointTypes) {
        appendArrayElement(OsduGeoJsonFeaturePropertiesElementLiterals::PointTypes, pointType);
    }
}

void
OsduGeoJsonFeaturePropertiesElement::initializeMetadataStruct()
{
    featurePropertyStruct_ = OsduGeoJsonFeaturePropertiesElementStruct(getOsduPartition());
}

void
OsduGeoJsonFeaturePropertiesElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    featurePropertyStruct_.lineName = getParameter<std::string>(OsduGeoJsonFeaturePropertiesElementLiterals::LineName);
    featurePropertyStruct_.pointTypes = getAllArrayElements<std::string>(OsduGeoJsonFeaturePropertiesElementLiterals::PointTypes);
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonFeaturePropertiesElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonFeaturePropertiesElementStruct>(featurePropertyStruct_);
}

} // namespace segysdk::metadata::osdu
