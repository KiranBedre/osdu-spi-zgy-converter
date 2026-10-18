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

#pragma once

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfoStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
/*
"FileSourceInfo": {
    "FileSource": "s3://default_bucket/r1/data/provided/documents/1000.witsml",
    "PreloadFilePath": "s3://staging-area/r7/raw-data/provided/documents/1000.witsml",
    "PreloadFileCreateUser": "somebody@acme.org",
    "PreloadFileCreateDate": "2019-12-16T11:46:20.163Z",
    "PreloadFileModifyUser": "somebody.else@acme.org",
    "PreloadFileModifyDate": "2019-12-20T17:20:05.356Z",
    "Name": "1000.witsml",
    "FileSize": "95463",
    "EncodingFormatTypeID": "namespace:reference-data--EncodingFormatType:application%2Fgeo%2Bjson:",
    "Checksum": "d41d8cd98f00b204e9800998ecf8427e",
    "ChecksumAlgorithm": "SHA-256"
  }
*/

class SEGYSDK_DLL_PUBLIC OsduAbstractFileSourceInfo final : public OsduMetadataBase
{
public:
    OsduAbstractFileSourceInfo(const std::string& osduPartition);
    OsduAbstractFileSourceInfo(const OsduAbstractFileSourceInfo& other) = default;
    OsduAbstractFileSourceInfo(OsduAbstractFileSourceInfo&& other) noexcept = default;
    OsduAbstractFileSourceInfo& operator=(const OsduAbstractFileSourceInfo& rhs) = default;
    OsduAbstractFileSourceInfo& operator=(OsduAbstractFileSourceInfo&& rhs) noexcept = default;
    ~OsduAbstractFileSourceInfo() override = default;

    OsduAbstractFileSourceInfo(OsduAbstractFileSourceInfoStruct fileSourceInfoStruct);
    explicit OsduAbstractFileSourceInfo(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

    OsduAbstractFileSourceInfoStruct fileSourceInfo_;
};
} // namespace segysdk::metadata::osdu
