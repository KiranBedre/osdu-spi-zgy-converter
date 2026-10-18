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

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfo.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfoLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractFileSourceInfo::OsduAbstractFileSourceInfo(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), fileSourceInfo_{OsduAbstractFileSourceInfoStruct(osduPartition)}
{
    /* Empty */
}

OsduAbstractFileSourceInfo::OsduAbstractFileSourceInfo(OsduAbstractFileSourceInfoStruct fileSourceInfoStruct)
    : OsduMetadataBase(fileSourceInfoStruct.getPartition()), fileSourceInfo_(std::move(fileSourceInfoStruct))
{
    populateJsonObject();
}

OsduAbstractFileSourceInfo::OsduAbstractFileSourceInfo(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileSourceInfo_(OsduAbstractFileSourceInfoStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractFileSourceInfo::isValidInput() const
{
    return fileSourceInfo_.isValid();
}


std::shared_ptr<OsduMetadataStructBase>
OsduAbstractFileSourceInfo::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractFileSourceInfoStruct>(fileSourceInfo_);
}

void
OsduAbstractFileSourceInfo::populateJsonObject()
{
    clearJsonObject();

    setParameter(OsduAbstractFileSourceInfoLiterals::FileSource, fileSourceInfo_.fileSource);

    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::Name, fileSourceInfo_.name);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::Checksum, fileSourceInfo_.checksum);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::ChecksumAlgorithm, fileSourceInfo_.checksumAlgorithm);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::EncodingFormatTypeID, fileSourceInfo_.encodingFormatTypeID);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::FileSize, fileSourceInfo_.fileSize);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::PreloadFilePath, fileSourceInfo_.preloadFilePath);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateUser, fileSourceInfo_.preloadFileCreateUser);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateDate, fileSourceInfo_.preloadFileCreateDate);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyUser, fileSourceInfo_.preloadFileModifyUser);
    setOptionalParameter(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyDate, fileSourceInfo_.preloadFileModifyDate);
}

void
OsduAbstractFileSourceInfo::initializeJson()
{
    removeMember(OsduAbstractFileSourceInfoLiterals::FileSource);
    removeMember(OsduAbstractFileSourceInfoLiterals::Checksum);
    removeMember(OsduAbstractFileSourceInfoLiterals::ChecksumAlgorithm);
    removeMember(OsduAbstractFileSourceInfoLiterals::EncodingFormatTypeID);
    removeMember(OsduAbstractFileSourceInfoLiterals::Name);
    removeMember(OsduAbstractFileSourceInfoLiterals::PreloadFilePath);
    removeMember(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateUser);
    removeMember(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateDate);
    removeMember(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyUser);
    removeMember(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyDate);
    removeMember(OsduAbstractFileSourceInfoLiterals::FileSize);
}

void
OsduAbstractFileSourceInfo::initializeMetadataStruct()
{
    fileSourceInfo_ = OsduAbstractFileSourceInfoStruct(getOsduPartition());
}

void
OsduAbstractFileSourceInfo::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileSourceInfo_.fileSource = getParameter<std::string>(OsduAbstractFileSourceInfoLiterals::FileSource);
    fileSourceInfo_.checksum = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::Checksum);
    fileSourceInfo_.checksumAlgorithm = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::ChecksumAlgorithm);
    fileSourceInfo_.encodingFormatTypeID = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::EncodingFormatTypeID);
    fileSourceInfo_.name = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::Name);
    fileSourceInfo_.preloadFilePath = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::PreloadFilePath);
    fileSourceInfo_.preloadFileCreateUser = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateUser);
    fileSourceInfo_.preloadFileCreateDate = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::PreloadFileCreateDate);
    fileSourceInfo_.preloadFileModifyUser = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyUser);
    fileSourceInfo_.preloadFileModifyDate = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::PreloadFileModifyDate);
    fileSourceInfo_.fileSize = getOptionalParameter<std::string>(OsduAbstractFileSourceInfoLiterals::FileSize);
}
} // namespace segysdk::metadata::osdu
