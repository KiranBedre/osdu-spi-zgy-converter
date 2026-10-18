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

#include "../OsduMetaDataUtilities/OsduAbstractDataset.h"
#include "../OsduMetaDataUtilities/OsduAbstractDatasetLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractDataset::OsduAbstractDataset(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), abstractDatasetStruct_(OsduAbstractDatasetStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractDataset::OsduAbstractDataset(OsduAbstractDatasetStruct abstractDatasetStruct)
    : OsduMetadataBase(abstractDatasetStruct.getPartition()), abstractDatasetStruct_(std::move(abstractDatasetStruct))
{
    populateJsonObject();
}

OsduAbstractDataset::OsduAbstractDataset(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), abstractDatasetStruct_(OsduAbstractDatasetStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractDataset::isValidInput() const
{
    return abstractDatasetStruct_.isValid();
}

void
OsduAbstractDataset::initializeJson()
{
    removeMember(OsduAbstractDatasetLiterals::Name);
    removeMember(OsduAbstractDatasetLiterals::Description);
    removeMember(OsduAbstractDatasetLiterals::EncodingFormatTypeID);
    removeMember(OsduAbstractDatasetLiterals::Endian);
    removeMember(OsduAbstractDatasetLiterals::SchemaFormatTypeID);
    removeMember(OsduAbstractDatasetLiterals::TotalSize);
    // DatasetProperties field can have a variaty of different specializations. Do not remove here
    //removeMember(OsduAbstractDatasetLiterals::DatasetProperties);

}

void
OsduAbstractDataset::populateJsonObject()
{
    // Do NOT clean Json object - it is in OsduDataElement space ...
    initializeJson();

    setOptionalParameter(OsduAbstractDatasetLiterals::Name, abstractDatasetStruct_.name);
    setOptionalParameter(OsduAbstractDatasetLiterals::Description, abstractDatasetStruct_.description);
    setOptionalParameter(OsduAbstractDatasetLiterals::EncodingFormatTypeID, abstractDatasetStruct_.encodingFormatTypeID);
    setOptionalParameter(OsduAbstractDatasetLiterals::SchemaFormatTypeID, abstractDatasetStruct_.schemaFormatTypeID);
    setOptionalParameter(OsduAbstractDatasetLiterals::TotalSize, abstractDatasetStruct_.totalSize);
    setOptionalParameter(OsduAbstractDatasetLiterals::Endian, abstractDatasetStruct_.endian);
}

void
OsduAbstractDataset::initializeMetadataStruct()
{
    abstractDatasetStruct_ = OsduAbstractDatasetStruct(getOsduPartition());
}

void
OsduAbstractDataset::populateMetadataStruct()
{
    initializeMetadataStruct();

    abstractDatasetStruct_.name = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::Name);
    abstractDatasetStruct_.description = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::Description);
    abstractDatasetStruct_.totalSize = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::TotalSize);
    abstractDatasetStruct_.encodingFormatTypeID = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::EncodingFormatTypeID);
    abstractDatasetStruct_.schemaFormatTypeID = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::SchemaFormatTypeID);
    abstractDatasetStruct_.endian = getOptionalParameter<std::string>(OsduAbstractDatasetLiterals::Endian);
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractDataset::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractDatasetStruct>(abstractDatasetStruct_);
}
} // namespace segysdk::metadata::osdu
