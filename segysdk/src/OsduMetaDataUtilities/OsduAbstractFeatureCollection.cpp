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

#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollection.h"
#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollectionLiterals.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeature.h"


namespace segysdk::metadata::osdu
{
OsduAbstractFeatureCollection::OsduAbstractFeatureCollection(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), FeatureCollectionStruct_(OsduAbstractFeatureCollectionStruct(osduPartition)) {}

OsduAbstractFeatureCollection::OsduAbstractFeatureCollection(const OsduAbstractFeatureCollection& other)
    : OsduMetadataBase(other.getOsduPartition()), FeatureCollectionStruct_(other.FeatureCollectionStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractFeatureCollection::OsduAbstractFeatureCollection(OsduAbstractFeatureCollection&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), FeatureCollectionStruct_(std::move(other.FeatureCollectionStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractFeatureCollection&
OsduAbstractFeatureCollection::operator=(const OsduAbstractFeatureCollection& rhs)
{
    return *this = OsduAbstractFeatureCollection(rhs);
}

OsduAbstractFeatureCollection&
OsduAbstractFeatureCollection::operator=(OsduAbstractFeatureCollection&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    FeatureCollectionStruct_ = std::move(rhs.FeatureCollectionStruct_);

    return *this;
}

OsduAbstractFeatureCollection::OsduAbstractFeatureCollection(OsduAbstractFeatureCollectionStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), FeatureCollectionStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduAbstractFeatureCollection::OsduAbstractFeatureCollection(Json::Value json, const std::string& osduPartition)
    : OsduMetadataBase(std::move(json), osduPartition), FeatureCollectionStruct_(OsduAbstractFeatureCollectionStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduAbstractFeatureCollection::isValidInput() const
{
    return FeatureCollectionStruct_.isValid();
}

void
OsduAbstractFeatureCollection::initializeJson()
{
    removeMember(OsduAbstractFeatureCollectionLiterals::type);
    removeMember(OsduAbstractFeatureCollectionLiterals::features);
    removeMember(OsduAbstractFeatureCollectionLiterals::bbox);    
}

void
OsduAbstractFeatureCollection::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduAbstractFeatureCollectionLiterals::type, FeatureCollectionStruct_.type);

    InitializeJsonArray(OsduAbstractFeatureCollectionLiterals::features);
    for (const auto& featureStruct : FeatureCollectionStruct_.features) {
        OsduGeoJsonFeature json(featureStruct);
        appendArrayElement(OsduAbstractFeatureCollectionLiterals::features, json.getAsJson());
    }

    if (FeatureCollectionStruct_.bbox.has_value()) {
        InitializeJsonArray(OsduAbstractFeatureCollectionLiterals::bbox);
        for (const auto value : FeatureCollectionStruct_.bbox.value()) {
            appendArrayElement(OsduAbstractFeatureCollectionLiterals::bbox, value);
        }
    }
}

void
OsduAbstractFeatureCollection::initializeMetadataStruct()
{
    FeatureCollectionStruct_ = OsduAbstractFeatureCollectionStruct(getOsduPartition());
}

void
OsduAbstractFeatureCollection::populateMetadataStruct()
{
    initializeMetadataStruct();

    FeatureCollectionStruct_.type = getParameter<std::string>(OsduAbstractFeatureCollectionLiterals::type);

    if (isMember(OsduAbstractFeatureCollectionLiterals::features)) {
        const auto arrayElements = getAllArrayElements<Json::Value>(OsduAbstractFeatureCollectionLiterals::features);
        for (const auto& json : arrayElements) {
            OsduGeoJsonFeature feature(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduGeoJsonFeatureStruct>(feature.getMetaDataStruct());
            if (ptr != nullptr) {
                FeatureCollectionStruct_.features.emplace_back(*ptr);
            }
        }
    }
    if (isMember(OsduAbstractFeatureCollectionLiterals::bbox)) {
        FeatureCollectionStruct_.bbox = getAllArrayElements<double>(OsduAbstractFeatureCollectionLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractFeatureCollection::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractFeatureCollectionStruct>(FeatureCollectionStruct_);
}

} // namespace segysdk::metadata::osdu
