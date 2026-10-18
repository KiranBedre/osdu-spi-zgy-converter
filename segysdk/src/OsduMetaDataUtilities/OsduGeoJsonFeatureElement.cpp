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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureElement.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureProperties.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureElementStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureElementLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGeoJsonFeatureElement::OsduGeoJsonFeatureElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), featurePropertyStruct_(OsduGeoJsonFeatureElementStruct(osduPartition)) {}

OsduGeoJsonFeatureElement::OsduGeoJsonFeatureElement(const OsduGeoJsonFeatureElement& other)
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(other.featurePropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeatureElement::OsduGeoJsonFeatureElement(OsduGeoJsonFeatureElement&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(std::move(other.featurePropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeatureElement&
OsduGeoJsonFeatureElement::operator=(const OsduGeoJsonFeatureElement& rhs)
{
    return *this = OsduGeoJsonFeatureElement(rhs);
}

OsduGeoJsonFeatureElement&
OsduGeoJsonFeatureElement::operator=(OsduGeoJsonFeatureElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    featurePropertyStruct_ = std::move(rhs.featurePropertyStruct_);

    return *this;
}

OsduGeoJsonFeatureElement::OsduGeoJsonFeatureElement(OsduGeoJsonFeatureElementStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), featurePropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonFeatureElement::OsduGeoJsonFeatureElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), featurePropertyStruct_(OsduGeoJsonFeatureElementStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduGeoJsonFeatureElement::isValidInput() const
{
    return featurePropertyStruct_.isValid();
}

void
OsduGeoJsonFeatureElement::initializeJson()
{
    removeMember(OsduGeoJsonFeatureElementLiterals::type);
    removeMember(OsduGeoJsonFeatureElementLiterals::geometry);
    removeMember(OsduGeoJsonFeatureElementLiterals::properties);
    removeMember(OsduGeoJsonFeatureElementLiterals::bbox);
}

void
OsduGeoJsonFeatureElement::populateJsonObject()
{
    clearJsonObject();

    setParameter(OsduGeoJsonFeatureElementLiterals::type, featurePropertyStruct_.type);


    if (featurePropertyStruct_.bbox.has_value()) {
        InitializeJsonArray(OsduGeoJsonFeatureElementLiterals::bbox);
        for (const auto& value : featurePropertyStruct_.bbox.value()) {
            appendArrayElement(OsduGeoJsonFeatureElementLiterals::bbox, value);
        }
    }
}


void
OsduGeoJsonFeatureElement::initializeMetadataStruct()
{
    featurePropertyStruct_ = OsduGeoJsonFeatureElementStruct(getOsduPartition());
}

void
OsduGeoJsonFeatureElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    featurePropertyStruct_.type = getParameter<std::string>(OsduGeoJsonFeatureElementLiterals::type);

    if (isMember(OsduGeoJsonFeatureElementLiterals::properties)) {

        OsduGeoJsonFeatureProperties properties(getParameter<Json::Value>(OsduGeoJsonFeatureElementLiterals::properties), getOsduPartition());
        const auto ptrStruct = std::reinterpret_pointer_cast<OsduGeoJsonFeaturePropertiesStruct>(properties.getMetaDataStruct());

        if (ptrStruct != nullptr) {
            featurePropertyStruct_.properties = *ptrStruct;
        }
    }

    if (isMember(OsduGeoJsonFeatureElementLiterals::geometry)) {

        OsduGeoJsonGeometryElement geometry(getParameter<Json::Value>(OsduGeoJsonFeatureElementLiterals::geometry), getOsduPartition());
        const auto ptrStruct = std::reinterpret_pointer_cast<OsduGeoJsonGeometryElementStruct>(geometry.getMetaDataStruct());

        if (ptrStruct != nullptr) {
            featurePropertyStruct_.geometry = *ptrStruct;
        }
    }


    if (isMember(OsduGeoJsonFeatureElementLiterals::bbox)) {
        featurePropertyStruct_.bbox = getAllArrayElements<double>(OsduGeoJsonFeatureElementLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonFeatureElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonFeatureElementStruct>(featurePropertyStruct_);
}

} // namespace segysdk::metadata::osdu
