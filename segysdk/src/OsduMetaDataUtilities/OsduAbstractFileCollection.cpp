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

#include "../OsduMetaDataUtilities/OsduAbstractFileCollection.h"

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfo.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileCollectionLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractFileCollection::OsduAbstractFileCollection(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), fileCollectionStruct_(OsduAbstractFileCollectionStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractFileCollection::OsduAbstractFileCollection(OsduAbstractFileCollectionStruct abstractFileCollectionStruct)
  : OsduMetadataBase(abstractFileCollectionStruct.getPartition(), false), fileCollectionStruct_(std::move(abstractFileCollectionStruct))
{
    populateJsonObject();
}

OsduAbstractFileCollection::OsduAbstractFileCollection(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileCollectionStruct_(OsduAbstractFileCollectionStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractFileCollection::isValidInput() const
{
    return fileCollectionStruct_.isValid();
}

void
OsduAbstractFileCollection::initializeJson()
{
    removeMember(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileCollectionPath);
    removeMember(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::IndexFilePath);
    removeMember(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileSourceInfos);
    removeMember(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::Checksum);
}

void
OsduAbstractFileCollection::populateJsonObject()
{
    initializeJson();

    setParameter(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileCollectionPath, fileCollectionStruct_.fileCollectionPath);
    setOptionalParameter(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::IndexFilePath, fileCollectionStruct_.indexFilePath);
    setOptionalArrayStructParameter<OsduAbstractFileSourceInfo>(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileSourceInfos, fileCollectionStruct_.fileSourceInfoStructs);
    setOptionalParameter(std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::Checksum, fileCollectionStruct_.checksum);
}

void
OsduAbstractFileCollection::initializeMetadataStruct()
{
    fileCollectionStruct_ = OsduAbstractFileCollectionStruct(getOsduPartition());
}

void
OsduAbstractFileCollection::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileCollectionStruct_.fileCollectionPath = getParameter<std::string>(
        std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileCollectionPath);
    fileCollectionStruct_.indexFilePath = getOptionalParameter<std::string>(
        std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::IndexFilePath);
    fileCollectionStruct_.fileSourceInfoStructs = getOptionalArrayStructParameter<OsduAbstractFileSourceInfo, OsduAbstractFileSourceInfoStruct>(
        std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::FileSourceInfos);
    fileCollectionStruct_.checksum = getOptionalParameter<std::string>(
        std::string(OsduAbstractFileCollectionLiterals::DatasetProperties) + ":" + OsduAbstractFileCollectionLiterals::Checksum);
}


std::shared_ptr<OsduMetadataStructBase>
OsduAbstractFileCollection::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractFileCollectionStruct>(fileCollectionStruct_);
}
} // namespace segysdk::metadata::osdu
