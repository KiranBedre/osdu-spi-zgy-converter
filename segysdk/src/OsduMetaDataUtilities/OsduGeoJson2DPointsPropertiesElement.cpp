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

#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElement.h"

#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElementStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElementLiterals.h"

namespace segysdk::metadata::osdu
{

OsduGeoJson2DPointsPropertiesElement::OsduGeoJson2DPointsPropertiesElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), points2DPropertyStruct_(OsduGeoJson2DPointsPropertiesElementStruct(osduPartition)) {}

OsduGeoJson2DPointsPropertiesElement::OsduGeoJson2DPointsPropertiesElement(const OsduGeoJson2DPointsPropertiesElement& other)
  : OsduMetadataBase(other.getOsduPartition()), points2DPropertyStruct_(other.points2DPropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJson2DPointsPropertiesElement::OsduGeoJson2DPointsPropertiesElement(OsduGeoJson2DPointsPropertiesElement&& other) noexcept
  : OsduMetadataBase(other.getOsduPartition()), points2DPropertyStruct_(std::move(other.points2DPropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJson2DPointsPropertiesElement&
OsduGeoJson2DPointsPropertiesElement::operator=(const OsduGeoJson2DPointsPropertiesElement& rhs)
{
    return *this = OsduGeoJson2DPointsPropertiesElement(rhs);
}

OsduGeoJson2DPointsPropertiesElement&
OsduGeoJson2DPointsPropertiesElement::operator=(OsduGeoJson2DPointsPropertiesElement&& rhs) noexcept
{
    setJson(rhs.getJson());

    points2DPropertyStruct_ = std::move(rhs.points2DPropertyStruct_);

    return *this;
}

OsduGeoJson2DPointsPropertiesElement::OsduGeoJson2DPointsPropertiesElement(OsduGeoJson2DPointsPropertiesElementStruct constantValueOverrideStruct)
  : OsduMetadataBase(constantValueOverrideStruct.getPartition()), points2DPropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJson2DPointsPropertiesElement::OsduGeoJson2DPointsPropertiesElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), points2DPropertyStruct_(OsduGeoJson2DPointsPropertiesElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGeoJson2DPointsPropertiesElement::isValidInput() const
{
    return points2DPropertyStruct_.isValid();
}

void
OsduGeoJson2DPointsPropertiesElement::initializeJson()
{
    removeMember(OsduGeoJson2DPointsPropertiesElementLiterals::ShotPointNumbers);
    removeMember(OsduGeoJson2DPointsPropertiesElementLiterals::CommonMidPointNumbers);
}

void
OsduGeoJson2DPointsPropertiesElement::populateJsonObject()
{
    clearJsonObject();

    if (!points2DPropertyStruct_.shotpointNumbers.empty()) {
        InitializeJsonArray(OsduGeoJson2DPointsPropertiesElementLiterals::ShotPointNumbers);
        for (const auto& number : points2DPropertyStruct_.shotpointNumbers) {
            appendArrayElement(OsduGeoJson2DPointsPropertiesElementLiterals::ShotPointNumbers, number);
        }
    }
    if (!points2DPropertyStruct_.commonMidPointNumbers.empty()) {
        InitializeJsonArray(OsduGeoJson2DPointsPropertiesElementLiterals::CommonMidPointNumbers);
        for (const auto& number : points2DPropertyStruct_.commonMidPointNumbers) {
            appendArrayElement(OsduGeoJson2DPointsPropertiesElementLiterals::CommonMidPointNumbers, number);
        }
    }
}

void
OsduGeoJson2DPointsPropertiesElement::initializeMetadataStruct()
{
    points2DPropertyStruct_ = OsduGeoJson2DPointsPropertiesElementStruct(getOsduPartition());
}

void
OsduGeoJson2DPointsPropertiesElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduGeoJson2DPointsPropertiesElementLiterals::ShotPointNumbers)) {
        points2DPropertyStruct_.shotpointNumbers = getAllArrayElements<double>(OsduGeoJson2DPointsPropertiesElementLiterals::ShotPointNumbers);
    }
    if (isMember(OsduGeoJson2DPointsPropertiesElementLiterals::CommonMidPointNumbers)) {
        points2DPropertyStruct_.commonMidPointNumbers = getAllArrayElements<double>(OsduGeoJson2DPointsPropertiesElementLiterals::CommonMidPointNumbers);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJson2DPointsPropertiesElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJson2DPointsPropertiesElementStruct>(points2DPropertyStruct_);
}

} // namespace segysdk::metadata::osdu
