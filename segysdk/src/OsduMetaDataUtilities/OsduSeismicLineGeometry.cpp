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

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometry.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryData.h"

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicLineGeometry::OsduSeismicLineGeometry(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicLineGeometryStruct_(OsduSeismicLineGeometryStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicLineGeometry::OsduSeismicLineGeometry(OsduSeismicLineGeometryStruct seismicLineGeometryStruct)
  : OsduMetadataBase(seismicLineGeometryStruct.getPartition()), seismicLineGeometryStruct_(std::move(seismicLineGeometryStruct))
{
    populateJsonObject();
}

OsduSeismicLineGeometry::OsduSeismicLineGeometry(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicLineGeometryStruct_(OsduSeismicLineGeometryStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicLineGeometry::isValidInput() const
{
    return seismicLineGeometryStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicLineGeometry::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicLineGeometryStruct>(seismicLineGeometryStruct_);
}

void
OsduSeismicLineGeometry::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicLineGeometry::populateJsonObject()
{
    initializeJson();

    OsduCommonMetadata commonMetadata(seismicLineGeometryStruct_.commonMetadataStruct);
    mergeInJson(commonMetadata.getAsJson());

    if (seismicLineGeometryStruct_.seismicLineGeometryDataStruct.has_value()) {
        Json::Value additionJson{Json::objectValue};
        additionJson[OsduSeismicLineGeometryLiterals::Data] = OsduSeismicLineGeometryData(seismicLineGeometryStruct_.seismicLineGeometryDataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduSeismicLineGeometry::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduSeismicLineGeometryLiterals::Id)) {
        const auto partition = OsduSeismicLineGeometryStruct::getPartitionFromReference(getParameter<std::string>(OsduSeismicLineGeometryLiterals::Id));
        setOsduPartition(partition);
    }

    seismicLineGeometryStruct_ = OsduSeismicLineGeometryStruct(getOsduPartition());
}

void
OsduSeismicLineGeometry::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicLineGeometryStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    seismicLineGeometryStruct_.seismicLineGeometryDataStruct = getOptionalStructParameter<OsduSeismicLineGeometryData, OsduSeismicLineGeometryDataStruct>(OsduSeismicLineGeometryLiterals::Data);
}
} // namespace segysdk::metadata::osdu
