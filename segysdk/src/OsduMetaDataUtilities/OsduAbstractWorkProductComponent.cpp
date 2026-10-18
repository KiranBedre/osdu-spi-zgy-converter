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

#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponent.h"

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"

#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentLiterals.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionData.h"
#include "../OsduMetaDataUtilities/OsduAbstractGeoContext.h"
#include "../OsduMetaDataUtilities/OsduLineageAssertion.h"


namespace segysdk::metadata::osdu
{
OsduAbstractWorkProductComponent::OsduAbstractWorkProductComponent(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    abstractWorkProductComponentStruct_(OsduAbstractWorkProductComponentStruct(osduPartition)) {}

OsduAbstractWorkProductComponent::OsduAbstractWorkProductComponent(const OsduAbstractWorkProductComponent& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    abstractWorkProductComponentStruct_(other.abstractWorkProductComponentStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractWorkProductComponent::OsduAbstractWorkProductComponent(OsduAbstractWorkProductComponent&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    abstractWorkProductComponentStruct_(std::move(other.abstractWorkProductComponentStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractWorkProductComponent&
OsduAbstractWorkProductComponent::operator=(const OsduAbstractWorkProductComponent& rhs)
{
    return *this = OsduAbstractWorkProductComponent(rhs);
}

OsduAbstractWorkProductComponent&
OsduAbstractWorkProductComponent::operator=(OsduAbstractWorkProductComponent&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    abstractWorkProductComponentStruct_ = std::move(rhs.abstractWorkProductComponentStruct_);

    return *this;
}

OsduAbstractWorkProductComponent::OsduAbstractWorkProductComponent(OsduAbstractWorkProductComponentStruct workProductComponentStruct)
  : OsduMetadataBase(workProductComponentStruct.getPartition(), false), abstractWorkProductComponentStruct_(std::move(workProductComponentStruct))
{
    populateJsonObject();
}

OsduAbstractWorkProductComponent::OsduAbstractWorkProductComponent(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), abstractWorkProductComponentStruct_(OsduAbstractWorkProductComponentStruct(osduPartition))
{
    populateMetadataStruct();
 }

bool
OsduAbstractWorkProductComponent::isValidInput() const
{
    return abstractWorkProductComponentStruct_.isValid();
}

void
OsduAbstractWorkProductComponent::initializeJson()
{
    removeMember(OsduAbstractWorkProductComponentLiterals::Name);
    removeMember(OsduAbstractWorkProductComponentLiterals::Description);
    removeMember(OsduAbstractWorkProductComponentLiterals::CreationDateTime);
    removeMember(OsduAbstractWorkProductComponentLiterals::Tags);
    removeMember(OsduAbstractWorkProductComponentLiterals::SpatialPoint);
    removeMember(OsduAbstractWorkProductComponentLiterals::SpatialArea);
    removeMember(OsduAbstractWorkProductComponentLiterals::GeoContexts);
    removeMember(OsduAbstractWorkProductComponentLiterals::SubmitterName);
    removeMember(OsduAbstractWorkProductComponentLiterals::AuthorIDs);
    removeMember(OsduAbstractWorkProductComponentLiterals::BusinessActivities);
    removeMember(OsduAbstractWorkProductComponentLiterals::LineageAssertions);
}

void
OsduAbstractWorkProductComponent::populateJsonObject()
{
    clearJsonObject();

    setOptionalParameter(OsduAbstractWorkProductComponentLiterals::Name, abstractWorkProductComponentStruct_.name);
    setOptionalParameter(OsduAbstractWorkProductComponentLiterals::Description, abstractWorkProductComponentStruct_.description);
    setOptionalParameter(OsduAbstractWorkProductComponentLiterals::CreationDateTime, abstractWorkProductComponentStruct_.creationDateTime);
    setOptionalParameter(OsduAbstractWorkProductComponentLiterals::SubmitterName, abstractWorkProductComponentStruct_.submitterName);

    if (abstractWorkProductComponentStruct_.tags.has_value()) {
        Json::Value tags;
        for (const auto& tag : abstractWorkProductComponentStruct_.tags.value()) {
            tags.append(tag);
        }
        setParameter(OsduAbstractWorkProductComponentLiterals::Tags, tags);
    }

    if (abstractWorkProductComponentStruct_.spatialPointStruct.has_value()) {
        OsduAbstractSpatialLocation loc(abstractWorkProductComponentStruct_.spatialPointStruct.value());
        setParameter(OsduAbstractWorkProductComponentLiterals::SpatialPoint, loc.getAsJson());
    }
    if (abstractWorkProductComponentStruct_.spatialAreaStruct.has_value()) {
        OsduAbstractSpatialLocation loc(abstractWorkProductComponentStruct_.spatialAreaStruct.value());
        setParameter(OsduAbstractWorkProductComponentLiterals::SpatialArea, loc.getAsJson());
    }
    if (abstractWorkProductComponentStruct_.geoContextsArray.has_value()) {

        Json::Value geoContexts;
        for (const auto& gconStruct : abstractWorkProductComponentStruct_.geoContextsArray.value()) {
            OsduAbstractGeoContext ctxt(gconStruct);
            geoContexts.append(ctxt.getAsJson());
        }

        setParameter(OsduAbstractWorkProductComponentLiterals::GeoContexts, geoContexts);
    }

    if (abstractWorkProductComponentStruct_.authorIDs.has_value()) {
        Json::Value authorIDs;
        for (const auto& author : abstractWorkProductComponentStruct_.authorIDs.value()) {
            authorIDs.append(author);
        }
        setParameter(OsduAbstractWorkProductComponentLiterals::AuthorIDs, authorIDs);
    }
    if (abstractWorkProductComponentStruct_.businessActivities.has_value()) {
        Json::Value activities;
        for (const auto& activity : abstractWorkProductComponentStruct_.businessActivities.value()) {
            activities.append(activity);
        }
        setParameter(OsduAbstractWorkProductComponentLiterals::BusinessActivities, activities);
    }

    if (abstractWorkProductComponentStruct_.lineageAssertions.has_value()) {

        Json::Value lineageAssertions;
        for (const auto& lineageStruct : abstractWorkProductComponentStruct_.lineageAssertions.value()) {
            OsduLineageAssertion lineage(lineageStruct);
            lineageAssertions.append(lineage.getAsJson());
        }

        setParameter(OsduAbstractWorkProductComponentLiterals::LineageAssertions, lineageAssertions);
    }
}

void
OsduAbstractWorkProductComponent::initializeMetadataStruct()
{
    abstractWorkProductComponentStruct_ = OsduAbstractWorkProductComponentStruct(getOsduPartition());
}

OsduAbstractSpatialLocationStruct
OsduAbstractWorkProductComponent::getSpatialStruct(const std::string& key) const
{
    if (isMember(key)) {
        OsduAbstractSpatialLocation loc(getParameter<Json::Value>(key), getOsduPartition());

        const auto ptr = std::reinterpret_pointer_cast<OsduAbstractSpatialLocationStruct>(loc.getMetaDataStruct());

        if (ptr != nullptr) {
            return *ptr;
        }
    }

    return OsduAbstractSpatialLocationStruct{getOsduPartition()};
}

std::vector<OsduAbstractGeoContextStruct>
OsduAbstractWorkProductComponent::getGetContexts() const
{
    std::vector<OsduAbstractGeoContextStruct> result;

    const auto contexts = getAllArrayElements<Json::Value>(OsduAbstractWorkProductComponentLiterals::GeoContexts);

    for (auto& jsonContext : contexts) {
        OsduAbstractGeoContext ctxt(jsonContext, getOsduPartition());
        const auto ptr = std::reinterpret_pointer_cast<OsduAbstractGeoContextStruct>(ctxt.getMetaDataStruct());

        if (ptr != nullptr) {
            result.emplace_back(*ptr);
        }
    }

    return result;
}

std::vector<OsduLineageAssertionStruct>
OsduAbstractWorkProductComponent::getGetLineageAssertions() const
{
    std::vector<OsduLineageAssertionStruct> result;
    const auto lineageVector = getAllArrayElements<Json::Value>(OsduAbstractWorkProductComponentLiterals::LineageAssertions);
    for (auto& jsonLineage : lineageVector) {
        OsduLineageAssertion lineage(jsonLineage, getOsduPartition());
        const auto ptr = std::reinterpret_pointer_cast<OsduLineageAssertionStruct>(lineage.getMetaDataStruct());
        if (ptr != nullptr) {
            result.emplace_back(*ptr);
        }
    }

    return result;
}

void
OsduAbstractWorkProductComponent::populateMetadataStruct()
{
    initializeMetadataStruct();

    abstractWorkProductComponentStruct_.name = getOptionalParameter<std::string>(OsduAbstractWorkProductComponentLiterals::Name);
    abstractWorkProductComponentStruct_.description = getOptionalParameter<std::string>(OsduAbstractWorkProductComponentLiterals::Description);
    abstractWorkProductComponentStruct_.creationDateTime = getOptionalParameter<std::string>(OsduAbstractWorkProductComponentLiterals::CreationDateTime);
    abstractWorkProductComponentStruct_.submitterName = getOptionalParameter<std::string>(OsduAbstractWorkProductComponentLiterals::SubmitterName);

    if (isMember(OsduAbstractWorkProductComponentLiterals::Tags)) {
        abstractWorkProductComponentStruct_.tags = getAllArrayElements<std::string>(OsduAbstractWorkProductComponentLiterals::Tags);
    }

    if (isMember(OsduAbstractWorkProductComponentLiterals::AuthorIDs)) {
        abstractWorkProductComponentStruct_.authorIDs = getAllArrayElements<std::string>(OsduAbstractWorkProductComponentLiterals::AuthorIDs);
    }

    if (isMember(OsduAbstractWorkProductComponentLiterals::BusinessActivities)) {
        abstractWorkProductComponentStruct_.businessActivities = getAllArrayElements<std::string>(OsduAbstractWorkProductComponentLiterals::BusinessActivities);
    }

    if (isMember(OsduAbstractWorkProductComponentLiterals::SpatialPoint)) {
        abstractWorkProductComponentStruct_.spatialPointStruct = getSpatialStruct(OsduAbstractWorkProductComponentLiterals::SpatialPoint);
    }
    if (isMember(OsduAbstractWorkProductComponentLiterals::SpatialArea)) {
        abstractWorkProductComponentStruct_.spatialAreaStruct = getSpatialStruct(OsduAbstractWorkProductComponentLiterals::SpatialArea);
    }

    if (isMember(OsduAbstractWorkProductComponentLiterals::GeoContexts)) {
        abstractWorkProductComponentStruct_.geoContextsArray = getGetContexts();
    }

     if (isMember(OsduAbstractWorkProductComponentLiterals::LineageAssertions)) {
        abstractWorkProductComponentStruct_.lineageAssertions = getGetLineageAssertions();
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractWorkProductComponent::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractWorkProductComponentStruct>(abstractWorkProductComponentStruct_);
}

} // namespace segysdk::metadata::osdu
