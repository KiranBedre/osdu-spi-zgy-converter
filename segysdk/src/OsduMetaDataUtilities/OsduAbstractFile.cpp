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

#include "../OsduMetaDataUtilities/OsduAbstractFile.h"

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfo.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileLiterals.h"

namespace segysdk::metadata::osdu
{

OsduAbstractFile::OsduAbstractFile(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), abstractFileStruct_(OsduAbstractFileStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractFile::OsduAbstractFile(OsduAbstractFileStruct commonResourcesStruct)
  : OsduMetadataBase(commonResourcesStruct.getPartition()), abstractFileStruct_(std::move(commonResourcesStruct))
{
    populateJsonObject();
}

OsduAbstractFile::OsduAbstractFile(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), abstractFileStruct_(OsduAbstractFileStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractFile::isValidInput() const
{
    return abstractFileStruct_.isValid();
}

void
OsduAbstractFile::initializeJson()
{
    // Remove only DatasetProperties:FileSourceInfo and Checksum fields
    removeMember(std::string(OsduAbstractFileLiterals::DatasetProperties) + ":" + OsduAbstractFileLiterals::FileSourceInfo);
    removeMember(OsduAbstractFileLiterals::Checksum);
}

void
OsduAbstractFile::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractFileLiterals::Checksum, abstractFileStruct_.checksum);
    setOptionalStructParameter<OsduAbstractFileSourceInfo>(
        std::string(OsduAbstractFileLiterals::DatasetProperties) + ":" + OsduAbstractFileLiterals::FileSourceInfo,
        abstractFileStruct_.fileSourceInfo);
}

void
OsduAbstractFile::initializeMetadataStruct()
{
    abstractFileStruct_ = OsduAbstractFileStruct(getOsduPartition());
}

void
OsduAbstractFile::populateMetadataStruct()
{
    initializeMetadataStruct();

    abstractFileStruct_.checksum = getOptionalParameter<std::string>(OsduAbstractFileLiterals::Checksum);
    abstractFileStruct_.fileSourceInfo = getOptionalStructParameter<OsduAbstractFileSourceInfo, OsduAbstractFileSourceInfoStruct>(
        std::string(OsduAbstractFileLiterals::DatasetProperties) + ":" + OsduAbstractFileLiterals::FileSourceInfo);
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractFile::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractFileStruct>(abstractFileStruct_);
}

} // namespace segysdk::metadata::osdu
