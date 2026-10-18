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

#include "../OsduMetaDataUtilities/OsduWorkProductData.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"
#include "../OsduMetaDataUtilities/OsduLineageAssertion.h"

#include "../OsduMetaDataUtilities/OsduWorkProductDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduWorkProductData::OsduWorkProductData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    workProductDataStruct_(osduPartition)
{
    /* Empty */
}

OsduWorkProductData::OsduWorkProductData(OsduWorkProductDataStruct workProductDataStruct)
  : OsduMetadataBase(workProductDataStruct.getPartition()), workProductDataStruct_(std::move(workProductDataStruct))
{
    populateJsonObject();
}

OsduWorkProductData::OsduWorkProductData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), workProductDataStruct_(OsduWorkProductDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduWorkProductData::isValidInput() const
{
    return workProductDataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduWorkProductData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduWorkProductDataStruct>(workProductDataStruct_);
}

void
OsduWorkProductData::initializeJson()
{
    clearJsonObject();
}

void
OsduWorkProductData::populateJsonObject()
{
    initializeJson();

    if (workProductDataStruct_.commonResources.has_value()) {
        mergeInJson(OsduAbstractCommonResources(workProductDataStruct_.commonResources.value()).getAsJson());
    }

    setOptionalArrayParameter(OsduWorkProductDataLiterals::Components, workProductDataStruct_.components);
    setOptionalParameter(OsduWorkProductDataLiterals::IsExtendedLoad, workProductDataStruct_.isExtendedLoad);
    setOptionalParameter(OsduWorkProductDataLiterals::IsDiscoverable, workProductDataStruct_.isDiscoverable);
    setOptionalParameter(OsduWorkProductDataLiterals::Name, workProductDataStruct_.name);
    setOptionalParameter(OsduWorkProductDataLiterals::Description, workProductDataStruct_.description);
    setOptionalParameter(OsduWorkProductDataLiterals::CreationDateTime, workProductDataStruct_.creationDateTime);
    setOptionalArrayParameter(OsduWorkProductDataLiterals::Tags, workProductDataStruct_.tags);

    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduWorkProductDataLiterals::SpatialPoint, workProductDataStruct_.spatialPointStruct);
    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduWorkProductDataLiterals::SpatialArea, workProductDataStruct_.spatialAreaStruct);

    setOptionalParameter(OsduWorkProductDataLiterals::SubmitterName, workProductDataStruct_.submitterName);
    setOptionalArrayParameter(OsduWorkProductDataLiterals::BusinessActivities, workProductDataStruct_.businessActivities);
    setOptionalArrayParameter(OsduWorkProductDataLiterals::AuthorIDs, workProductDataStruct_.authorIDs);

    setOptionalArrayStructParameter<OsduLineageAssertion>(OsduWorkProductDataLiterals::LineageAssertions, workProductDataStruct_.lineageAssertionsStruct);

    setOptionalArrayParameter(OsduWorkProductDataLiterals::Annotations, workProductDataStruct_.annotations);
}

void
OsduWorkProductData::initializeMetadataStruct()
{
    workProductDataStruct_ = OsduWorkProductDataStruct(getOsduPartition());
}

void
OsduWorkProductData::populateMetadataStruct()
{
    initializeMetadataStruct();

    workProductDataStruct_.commonResources = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();

    workProductDataStruct_.components= getOptionalArrayParameter<std::string>(OsduWorkProductDataLiterals::Components);
    workProductDataStruct_.isExtendedLoad = getOptionalParameter<bool>(OsduWorkProductDataLiterals::IsExtendedLoad);
    workProductDataStruct_.isDiscoverable = getOptionalParameter<bool>(OsduWorkProductDataLiterals::IsDiscoverable);
    workProductDataStruct_.name = getOptionalParameter<std::string>(OsduWorkProductDataLiterals::Name);
    workProductDataStruct_.description = getOptionalParameter<std::string>(OsduWorkProductDataLiterals::Description);
    workProductDataStruct_.creationDateTime = getOptionalParameter<std::string>(OsduWorkProductDataLiterals::CreationDateTime);
    workProductDataStruct_.tags = getOptionalArrayParameter<std::string>(OsduWorkProductDataLiterals::Tags);

    workProductDataStruct_.spatialPointStruct = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduWorkProductDataLiterals::SpatialPoint);
    workProductDataStruct_.spatialAreaStruct = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduWorkProductDataLiterals::SpatialArea);

    workProductDataStruct_.submitterName = getOptionalParameter<std::string>(OsduWorkProductDataLiterals::SubmitterName);
    workProductDataStruct_.businessActivities = getOptionalArrayParameter<std::string>(OsduWorkProductDataLiterals::BusinessActivities);
    workProductDataStruct_.authorIDs = getOptionalArrayParameter<std::string>(OsduWorkProductDataLiterals::AuthorIDs);

    workProductDataStruct_.lineageAssertionsStruct = getOptionalArrayStructParameter<OsduLineageAssertion, OsduLineageAssertionStruct>(OsduWorkProductDataLiterals::LineageAssertions);

    workProductDataStruct_.annotations = getOptionalArrayParameter<std::string>(OsduWorkProductDataLiterals::Annotations);
}
} // namespace segysdk::metadata::osdu
