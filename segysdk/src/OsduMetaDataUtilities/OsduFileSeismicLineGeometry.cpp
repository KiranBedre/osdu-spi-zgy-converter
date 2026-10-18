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

#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometry.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryData.h"

#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileSeismicLineGeometry::OsduFileSeismicLineGeometry(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileSeismicLineGeometryStruct_(OsduFileSeismicLineGeometryStruct(osduPartition))
{
    /* Empty */
}

OsduFileSeismicLineGeometry::OsduFileSeismicLineGeometry(OsduFileSeismicLineGeometryStruct fileSeismicLineGeometryStruct)
  : OsduMetadataBase(fileSeismicLineGeometryStruct.getPartition()), fileSeismicLineGeometryStruct_(std::move(fileSeismicLineGeometryStruct))
{
    populateJsonObject();
}

OsduFileSeismicLineGeometry::OsduFileSeismicLineGeometry(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileSeismicLineGeometryStruct_(OsduFileSeismicLineGeometryStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileSeismicLineGeometry::isValidInput() const
{
    return fileSeismicLineGeometryStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileSeismicLineGeometry::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileSeismicLineGeometryStruct>(fileSeismicLineGeometryStruct_);
}

void
OsduFileSeismicLineGeometry::initializeJson()
{
    clearJsonObject();
}

void
OsduFileSeismicLineGeometry::populateJsonObject()
{
    initializeJson();

    OsduCommonMetadata commonMetadata(fileSeismicLineGeometryStruct_.commonMetadataStruct);
    mergeInJson(commonMetadata.getAsJson());

    if (fileSeismicLineGeometryStruct_.fileSeismicLineGeometryDataStruct.has_value()) {
        Json::Value additionJson{Json::objectValue};
        additionJson[OsduFileSeismicLineGeometryLiterals::Data] = OsduFileSeismicLineGeometryData(fileSeismicLineGeometryStruct_.fileSeismicLineGeometryDataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduFileSeismicLineGeometry::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduFileSeismicLineGeometryLiterals::Id)) {
        const auto partition = OsduFileSeismicLineGeometryStruct::getPartitionFromReference(getParameter<std::string>(OsduFileSeismicLineGeometryLiterals::Id));
        setOsduPartition(partition);
    }

    fileSeismicLineGeometryStruct_ = OsduFileSeismicLineGeometryStruct(getOsduPartition());
}

void
OsduFileSeismicLineGeometry::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileSeismicLineGeometryStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    fileSeismicLineGeometryStruct_.fileSeismicLineGeometryDataStruct = getOptionalStructParameter<OsduFileSeismicLineGeometryData, OsduFileSeismicLineGeometryDataStruct>(OsduFileSeismicLineGeometryLiterals::Data);
}
} // namespace segysdk::metadata::osdu
