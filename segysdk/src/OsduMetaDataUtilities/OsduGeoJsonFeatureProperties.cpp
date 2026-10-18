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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureProperties.h"

#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonLabelPropertiesElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonPointPropertiesElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesLiterals.h"

#include <cpprest/json.h>

namespace segysdk::metadata::osdu
{
OsduGeoJsonFeatureProperties::OsduGeoJsonFeatureProperties(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), featurePropertyStruct_(OsduGeoJsonFeaturePropertiesStruct(osduPartition)) {}

OsduGeoJsonFeatureProperties::OsduGeoJsonFeatureProperties(const OsduGeoJsonFeatureProperties& other)
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(other.featurePropertyStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeatureProperties::OsduGeoJsonFeatureProperties(OsduGeoJsonFeatureProperties&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), featurePropertyStruct_(std::move(other.featurePropertyStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonFeatureProperties&
OsduGeoJsonFeatureProperties::operator=(const OsduGeoJsonFeatureProperties& rhs)
{
    return *this = OsduGeoJsonFeatureProperties(rhs);
}

OsduGeoJsonFeatureProperties&
OsduGeoJsonFeatureProperties::operator=(OsduGeoJsonFeatureProperties&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());

   featurePropertyStruct_ =  std::move(rhs.featurePropertyStruct_);

    setJson(rhs.getAsJson());
    

    return *this;
}

OsduGeoJsonFeatureProperties::OsduGeoJsonFeatureProperties(OsduGeoJsonFeaturePropertiesStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), featurePropertyStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonFeatureProperties::OsduGeoJsonFeatureProperties(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), featurePropertyStruct_(OsduGeoJsonFeaturePropertiesStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduGeoJsonFeatureProperties::isValidInput() const
{
    return featurePropertyStruct_.isValid();
}

void
OsduGeoJsonFeatureProperties::initializeJson()
{
    assignRootJsonObject(Json::objectValue);
}

void
OsduGeoJsonFeatureProperties::populateJsonObject()
{
    initializeJson();

    if (featurePropertyStruct_.ijNumbers.has_value()) {
        InitializeJsonArray(OsduGeoJsonFeaturePropertiesLiterals::ijNumbers);
        for (const auto& pair : featurePropertyStruct_.ijNumbers.value()) {
            Json::Value json;
            for (const auto& value : pair) {
                json.append(value);
            }
            appendArrayElement(OsduGeoJsonFeaturePropertiesLiterals::ijNumbers, json);
        }
    }

    if (featurePropertyStruct_.pointProperties.has_value()) {
        InitializeJsonArray(OsduGeoJsonFeaturePropertiesLiterals::PointProperties);
        for (const auto& element : featurePropertyStruct_.pointProperties.value()) {
            
            OsduGeoJsonPointPropertiesElement point(element);
            appendArrayElement(OsduGeoJsonFeaturePropertiesLiterals::PointProperties, point.getAsJson());
        }
    }
    if (featurePropertyStruct_.points2DProperties.has_value()) {
        OsduGeoJson2DPointsPropertiesElement points2D(featurePropertyStruct_.points2DProperties.value());
        setParameter(OsduGeoJsonFeaturePropertiesLiterals::PointsProperties, points2D.getAsJson());
        
    }
    if (featurePropertyStruct_.featureProperties.has_value()) {
        OsduGeoJsonFeaturePropertiesElement featureProp(featurePropertyStruct_.featureProperties.value());

        setParameter(OsduGeoJsonFeaturePropertiesLiterals::FeatureProperties, featureProp.getAsJson());
    }

    if (featurePropertyStruct_.labelProperties.has_value()) {
        for (const auto& element : featurePropertyStruct_.labelProperties.value()) {
            if (!element.attributeName.empty()) {
                if (element.boolValue.has_value()) {
                    setOptionalParameter(element.attributeName, element.boolValue);
                }
                if (element.stringValue.has_value()) {
                    setOptionalParameter(element.attributeName, element.stringValue);
                }
                if (element.intValue.has_value()) {
                    setOptionalParameter(element.attributeName, element.intValue);
                }
                if (element.doubleValue.has_value()) {
                    setOptionalParameter(element.attributeName, element.doubleValue);
                }
            }
        }
    }
}

void
OsduGeoJsonFeatureProperties::initializeMetadataStruct()
{
    featurePropertyStruct_ = OsduGeoJsonFeaturePropertiesStruct(getOsduPartition());
}

void
OsduGeoJsonFeatureProperties::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduGeoJsonFeaturePropertiesLiterals::ijNumbers)) {
        const auto array = getAllArrayElements <Json::Value>(OsduGeoJsonFeaturePropertiesLiterals::ijNumbers);
        std::vector<std::vector<int>> result;
        for (const auto& pair : array) {
            std::vector<int> pairs;
            for (const auto& pp : pair) {
                pairs.emplace_back(pp.asInt());
            }
            result.emplace_back(pairs);
        }
        featurePropertyStruct_.ijNumbers = result;
    }

    if (isMember(OsduGeoJsonFeaturePropertiesLiterals::PointProperties)) {
        const auto array = getAllArrayElements<Json::Value>(OsduGeoJsonFeaturePropertiesLiterals::PointProperties);
        std::vector<OsduGeoJsonPointPropertiesElementStruct> result;
        for (const auto& pp : array) {
            OsduGeoJsonPointPropertiesElement point(pp, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduGeoJsonPointPropertiesElementStruct>(point.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }

        featurePropertyStruct_.pointProperties = result;
    }

    featurePropertyStruct_.featureProperties = getOptionalStructParameter<OsduGeoJsonFeaturePropertiesElement, OsduGeoJsonFeaturePropertiesElementStruct>(OsduGeoJsonFeaturePropertiesLiterals::FeatureProperties);
    featurePropertyStruct_.points2DProperties = getOptionalStructParameter<OsduGeoJson2DPointsPropertiesElement, OsduGeoJson2DPointsPropertiesElementStruct>(OsduGeoJsonFeaturePropertiesLiterals::PointsProperties);

    std::vector<OsduGeoJsonLabelPropertiesElementStruct> labelVector;
    const auto memberList = getJsonMemberNames();
    for (const auto& member : memberList) {
        if (member == OsduGeoJsonFeaturePropertiesLiterals::ijNumbers or member == OsduGeoJsonFeaturePropertiesLiterals::PointProperties or member == OsduGeoJsonFeaturePropertiesLiterals::Inline or member == OsduGeoJsonFeaturePropertiesLiterals::Crossline or member == OsduGeoJsonFeaturePropertiesLiterals::FeatureProperties or member == OsduGeoJsonFeaturePropertiesLiterals::PointsProperties) {
            continue;
        }

        Json::Value label;
        label[member] = getJson()[member];

        OsduGeoJsonLabelPropertiesElement element(label, getOsduPartition());
        const auto elementStruct = std::reinterpret_pointer_cast<OsduGeoJsonLabelPropertiesElementStruct>(element.getMetaDataStruct());
        if (elementStruct != nullptr) {
            labelVector.emplace_back(*elementStruct);
        }
    }
    if (!labelVector.empty()) {
        featurePropertyStruct_.labelProperties = labelVector;
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonFeatureProperties::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonFeaturePropertiesStruct>(featurePropertyStruct_);
}

} // namespace segysdk::metadata::osdu
