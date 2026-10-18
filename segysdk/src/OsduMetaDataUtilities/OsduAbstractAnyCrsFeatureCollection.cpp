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

#include "../OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollection.h"
#include "../OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollectionLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonFeature.h"


namespace segysdk::metadata::osdu
{
OsduAbstractAnyCrsFeatureCollection::OsduAbstractAnyCrsFeatureCollection(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), anyCrsFeatureCollectionStruct_(OsduAbstractAnyCrsFeatureCollectionStruct(osduPartition)) {}

OsduAbstractAnyCrsFeatureCollection::OsduAbstractAnyCrsFeatureCollection(const OsduAbstractAnyCrsFeatureCollection& other)
    : OsduMetadataBase(other.getOsduPartition()), anyCrsFeatureCollectionStruct_(other.anyCrsFeatureCollectionStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractAnyCrsFeatureCollection::OsduAbstractAnyCrsFeatureCollection(OsduAbstractAnyCrsFeatureCollection&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), anyCrsFeatureCollectionStruct_(std::move(other.anyCrsFeatureCollectionStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractAnyCrsFeatureCollection&
OsduAbstractAnyCrsFeatureCollection::operator=(const OsduAbstractAnyCrsFeatureCollection& rhs)
{
    return *this = OsduAbstractAnyCrsFeatureCollection(rhs);
}

OsduAbstractAnyCrsFeatureCollection&
OsduAbstractAnyCrsFeatureCollection::operator=(OsduAbstractAnyCrsFeatureCollection&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    anyCrsFeatureCollectionStruct_ = std::move(rhs.anyCrsFeatureCollectionStruct_);

    return *this;
}

OsduAbstractAnyCrsFeatureCollection::OsduAbstractAnyCrsFeatureCollection(OsduAbstractAnyCrsFeatureCollectionStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), anyCrsFeatureCollectionStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduAbstractAnyCrsFeatureCollection::OsduAbstractAnyCrsFeatureCollection(Json::Value json, const std::string& osduPartition)
    : OsduMetadataBase(std::move(json), osduPartition), anyCrsFeatureCollectionStruct_(OsduAbstractAnyCrsFeatureCollectionStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduAbstractAnyCrsFeatureCollection::isValidInput() const
{
    return anyCrsFeatureCollectionStruct_.isValid();
}

void
OsduAbstractAnyCrsFeatureCollection::initializeJson()
{
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::type);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::CoordinateReferenceSystemID);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalCoordinateReferenceSystemID);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalUnitID);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceCrs);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceVerticalCrs);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceUnitZ);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::features);
    removeMember(OsduAbstractAnyCrsFeatureCollectionLiterals::bbox);    
}

void
OsduAbstractAnyCrsFeatureCollection::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::type, anyCrsFeatureCollectionStruct_.type);
    setOptionalParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::CoordinateReferenceSystemID, anyCrsFeatureCollectionStruct_.coordinateReferenceSystemID);
    setOptionalParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalCoordinateReferenceSystemID, anyCrsFeatureCollectionStruct_.verticalCoordinateReferenceSystemID);
    setOptionalParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalUnitID, anyCrsFeatureCollectionStruct_.verticalUnitID);
    setParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceCrs, anyCrsFeatureCollectionStruct_.persistableReferenceCrs);
    setOptionalParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceVerticalCrs, anyCrsFeatureCollectionStruct_.persistableReferenceVerticalCrs);
    setOptionalParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceUnitZ, anyCrsFeatureCollectionStruct_.persistableReferenceUnitZ);

    Json::Value jsonFeatures;
    for (const auto& featureStruct : anyCrsFeatureCollectionStruct_.features) {
        OsduAnyCrsGeoJsonFeature json(featureStruct);
        jsonFeatures.append(json.getAsJson());
    }
    setParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::features, jsonFeatures);

    if (anyCrsFeatureCollectionStruct_.bbox.has_value()) {
        Json::Value bbox;
        for (const auto value : anyCrsFeatureCollectionStruct_.bbox.value()) {
            bbox.append(value);
        }
        setParameter(OsduAbstractAnyCrsFeatureCollectionLiterals::bbox, bbox);
    }
}

void
OsduAbstractAnyCrsFeatureCollection::initializeMetadataStruct()
{
    anyCrsFeatureCollectionStruct_ = OsduAbstractAnyCrsFeatureCollectionStruct(getOsduPartition());
}

void
OsduAbstractAnyCrsFeatureCollection::populateMetadataStruct()
{
    initializeMetadataStruct();

    anyCrsFeatureCollectionStruct_.type = getParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::type);
    anyCrsFeatureCollectionStruct_.coordinateReferenceSystemID = getOptionalParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::CoordinateReferenceSystemID);
    anyCrsFeatureCollectionStruct_.verticalCoordinateReferenceSystemID = getOptionalParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalCoordinateReferenceSystemID);
    anyCrsFeatureCollectionStruct_.verticalUnitID = getOptionalParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::VerticalUnitID);
    anyCrsFeatureCollectionStruct_.persistableReferenceCrs = getParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceCrs);
    anyCrsFeatureCollectionStruct_.persistableReferenceVerticalCrs = getOptionalParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceVerticalCrs);
    anyCrsFeatureCollectionStruct_.persistableReferenceUnitZ = getOptionalParameter<std::string>(OsduAbstractAnyCrsFeatureCollectionLiterals::persistableReferenceUnitZ);

    if (isMember(OsduAbstractAnyCrsFeatureCollectionLiterals::features)) {
        const auto arrayElements = getAllArrayElements<Json::Value>(OsduAbstractAnyCrsFeatureCollectionLiterals::features);
        for (const auto& json : arrayElements) {
            OsduAnyCrsGeoJsonFeature feature(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAnyCrsGeoJsonFeatureStruct>(feature.getMetaDataStruct());
            if (ptr != nullptr) {
                anyCrsFeatureCollectionStruct_.features.emplace_back(*ptr);
            }
        }
    }
    if (isMember(OsduAbstractAnyCrsFeatureCollectionLiterals::bbox)) {
        anyCrsFeatureCollectionStruct_.bbox = getAllArrayElements<double>(OsduAbstractAnyCrsFeatureCollectionLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractAnyCrsFeatureCollection::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractAnyCrsFeatureCollectionStruct>(anyCrsFeatureCollectionStruct_);
}

} // namespace segysdk::metadata::osdu
