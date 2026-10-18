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

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryRef.h"

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryRefLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicLineGeometryRef::OsduSeismicLineGeometryRef(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicLineGeometryRefStruct_(osduPartition)
{
    /* Empty */
}
OsduSeismicLineGeometryRef::OsduSeismicLineGeometryRef(OsduSeismicLineGeometryRefStruct seismicLineGeometryRefStruct)
  : OsduMetadataBase(seismicLineGeometryRefStruct.getPartition()), seismicLineGeometryRefStruct_(std::move(seismicLineGeometryRefStruct))
{
    populateJsonObject();
}

OsduSeismicLineGeometryRef::OsduSeismicLineGeometryRef(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicLineGeometryRefStruct_(OsduSeismicLineGeometryRefStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicLineGeometryRef::isValidInput() const
{
    return seismicLineGeometryRefStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicLineGeometryRef::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicLineGeometryRefStruct>(seismicLineGeometryRefStruct_);
}

void
OsduSeismicLineGeometryRef::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicLineGeometryRef::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduSeismicLineGeometryRefLiterals::SeismicLineGeometryID, seismicLineGeometryRefStruct_.id);
    setOptionalParameter(OsduSeismicLineGeometryRefLiterals::SeismicLineName, seismicLineGeometryRefStruct_.name);
}

void
OsduSeismicLineGeometryRef::initializeMetadataStruct()
{
    seismicLineGeometryRefStruct_ = OsduSeismicLineGeometryRefStruct(getOsduPartition());
}

void
OsduSeismicLineGeometryRef::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicLineGeometryRefStruct_.id = getOptionalParameter<std::string>(OsduSeismicLineGeometryRefLiterals::SeismicLineGeometryID);
    seismicLineGeometryRefStruct_.name = getOptionalParameter<std::string>(OsduSeismicLineGeometryRefLiterals::SeismicLineName);
}
} // namespace segysdk::metadata::osdu
