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

#include "../OsduMetaDataUtilities/OsduDataElement.h"
#include "../OsduMetaDataUtilities/OsduDataElementLiterals.h"

#include "../OsduMetaDataUtilities/OsduDatasetProperties.h"

namespace segysdk::metadata::osdu
{


OsduDataElement::OsduDataElement(OsduDataElementStruct dataElementStruct)
  : OsduMetadataBase(false), dataElementStruct_(std::move(dataElementStruct)), helper_(dataElementStruct_.getPartition())
{
    populateJsonObject();
}

OsduDataElement::OsduDataElement(const Json::Value& json)
{
    assignRootJsonObject(json);

    if (json.isMember(OsduDataElementLiterals::Checksum)) {
        dataElementStruct_.checksum = json[OsduDataElementLiterals::Checksum].asString();
    }

    /*
      
    if (json.isMember(OsduDataLiterals::Endian)) {
        dataElementStruct_.endian = json[OsduDataLiterals::Endian].asString();
    }
    if (json.isMember(OsduDataLiterals::Name)) {
        dataElementStruct_.name = json[OsduDataLiterals::Name].asString();
    }
    
   
    if (json.isMember(OsduDataLiterals::ResourceHomeRegionID)) {
        dataElementStruct_.resourceHomeRegionID = json[OsduDataLiterals::ResourceHomeRegionID].asString();
    }
    if (json.isMember(OsduDataLiterals::ResourceHostRegionIDs)) {
        std::vector<std::string> countries;
        for (const auto& region : json[OsduDataLiterals::ResourceHostRegionIDs]) {
            countries.emplace_back(region.asString());
        }
        dataElementStruct_.resourceHomeRegions = countries;
    }
    
    */

    populateJsonObject();
}

void
OsduDataElement::populateJsonObject()
{
    clearJsonObject();

    if (dataElementStruct_.resourceHomeRegionID.has_value()) {
        setParameter(OsduDataElementLiterals::ResourceHomeRegionArea, dataElementStruct_.resourceHomeRegionID.value());
        if (dataElementStruct_.resourceHomeRegions.has_value()) {
            for (const auto& country : dataElementStruct_.resourceHomeRegions.value()) {
                addArrayElement(OsduDataElementLiterals:: , dataElementStruct_.resourceHomeRegions.value());
            }
    }

    /*
    if (dataElementStruct_.description.has_value()) {
        setParameter(OsduDataLiterals::Description, dataElementStruct_.description.value());
    }

    if (dataElementStruct_.datasetProperties.isValid()) {
        const OsduDatasetProperties properties(dataElementStruct_.datasetProperties);
        setParameter(OsduDataLiterals::DatasetProperties, properties.getAsJson());
    }
    if (dataElementStruct_.totalSize.has_value() && dataElementStruct_.totalSize.value() > 0) {
        setParameter(OsduDataLiterals::TotalSize, dataElementStruct_.totalSize.value());
    }
    */
}

void OsduDataElement::initializeJson()
{
}

void
OsduDataElement::initializeMetadataStruct()
{
    dataElementStruct_ = OsduDataStruct();
}

bool
OsduDataElement::isValidInput() const 
{
    return dataElementStruct_.isValid();
    
}

std::shared_ptr<OsduMetadataStructBase>
OsduDataElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduDataStruct>(dataElementStruct_);
}

void
OsduDataElement::setResourceHomeRegionID(const std::string& home_region)
{
    // "ResourceHomeRegionID": "namespace:reference-data--OSDURegion:AWSEastUSA:",
    if (home_region.find("")
    setParameter(OsduDataLiterals::ResourceHomeRegionID, helper_.osduReferenceLookupString(OsduDataLiterals::ResourceHomeRegionArea, home_region));
}


void
OsduDataElement::setResourceHomeRegionIDs(const std::vector<std::string>& home_regions)
{
    // "ResourceHomeRegionIDs": ["namespace:reference-data--OSDURegion:AWSEastUSA:"],
    for (const auto& home : home_regions) {
        addArrayElement(OsduDataLiterals::ResourceHostRegionIDs, helper_.osduReferenceLookupString(OsduDataLiterals::ResourceHomeRegionArea, home));
    }
}



} // namespace segysdk::metadata::osdu
