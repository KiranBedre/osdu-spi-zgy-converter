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

#include "../OsduMetaDataUtilities/OsduAbstractGeoContext.h"

#include "../OsduMetaDataUtilities/OsduAbstractGeoContextLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractGeoContext::OsduAbstractGeoContext(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    geoContextStruct_(OsduAbstractGeoContextStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractGeoContext::OsduAbstractGeoContext(OsduAbstractGeoContextStruct commonResourceStruct)
    : OsduMetadataBase(commonResourceStruct.getPartition()), geoContextStruct_(std::move(commonResourceStruct))
{
    populateJsonObject();
}


OsduAbstractGeoContext::OsduAbstractGeoContext(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition),
    geoContextStruct_(OsduAbstractGeoContextStruct(osduPartition))
{
    populateMetadataStruct();
}

OsduAbstractGeoContext::OsduAbstractGeoContext(const OsduAbstractGeoContext& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      geoContextStruct_(other.geoContextStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractGeoContext::OsduAbstractGeoContext(OsduAbstractGeoContext&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      geoContextStruct_(std::move(other.geoContextStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractGeoContext&
OsduAbstractGeoContext::operator=(const OsduAbstractGeoContext& rhs)
{
    return *this = OsduAbstractGeoContext(rhs);
}

OsduAbstractGeoContext&
OsduAbstractGeoContext::operator=(OsduAbstractGeoContext&& rhs) noexcept
{
    geoContextStruct_ = std::move(rhs.geoContextStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    return *this;
}

bool
OsduAbstractGeoContext::isValidInput() const
{
    return geoContextStruct_.isValid();
}


void
OsduAbstractGeoContext::initializeJson()
{
    removeMember(OsduAbstractGeoContextLiterals::GeoTypeID);
    removeMember(OsduAbstractGeoContextLiterals::GeoPoliticalEntityID);
    removeMember(OsduAbstractGeoContextLiterals::BasinID);
    removeMember(OsduAbstractGeoContextLiterals::FieldID);
    removeMember(OsduAbstractGeoContextLiterals::PlayID);
    removeMember(OsduAbstractGeoContextLiterals::ProspectID);
}

void
OsduAbstractGeoContext::populateJsonObject()
{
    // Do NOT clean Json object - it is in OsduDataElement space ...
    initializeJson();
    setOptionalParameter(OsduAbstractGeoContextLiterals::GeoTypeID, geoContextStruct_.geoTypeID);
    setOptionalParameter(OsduAbstractGeoContextLiterals::GeoPoliticalEntityID, geoContextStruct_.geoPoliticalEntityID);
    setOptionalParameter(OsduAbstractGeoContextLiterals::BasinID, geoContextStruct_.basinID);
    setOptionalParameter(OsduAbstractGeoContextLiterals::FieldID, geoContextStruct_.fieldID);
    setOptionalParameter(OsduAbstractGeoContextLiterals::PlayID, geoContextStruct_.playID);
    setOptionalParameter(OsduAbstractGeoContextLiterals::ProspectID, geoContextStruct_.prospectID);
    
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractGeoContext::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractGeoContextStruct>(geoContextStruct_);
}

void
OsduAbstractGeoContext::initializeMetadataStruct()
{
    geoContextStruct_ = OsduAbstractGeoContextStruct(getOsduPartition());
}

void
OsduAbstractGeoContext::populateMetadataStruct()
{
    initializeMetadataStruct();

    geoContextStruct_.geoTypeID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::GeoTypeID);
    geoContextStruct_.geoPoliticalEntityID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::GeoPoliticalEntityID);
    geoContextStruct_.basinID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::BasinID);
    geoContextStruct_.fieldID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::FieldID);
    geoContextStruct_.playID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::PlayID);
    geoContextStruct_.prospectID = getOptionalParameter<std::string>(OsduAbstractGeoContextLiterals::ProspectID);
}

} // namespace segysdk::metadata::osdu
