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

#include "../OsduMetaDataUtilities/OsduAbstractFileCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
/*
      "FileSourceInfos": [
        {
          "Checksum": "F8AEFDD98D8B5C9EBA1F791DE12D44B2",
          "ChecksumAlgorithm": "MD5",
          "FileSource": "E:\\Seismic\\SEGY2D\\Section2D\\section.sgy",
          "PreloadFilePath": "MyDocuments\\seismic-to-osdu\\Section2D\\section.sgy",
          "Name": "Section2D",
          "FileSize": "2076672"
        }
      ],
      "Checksum": "F8AEFDD98D8B5C9EBA1F791DE12D44B2"
*/

class SEGYSDK_DLL_PUBLIC OsduAbstractFileCollection final : public OsduMetadataBase
{
public:
    OsduAbstractFileCollection(const std::string& osduPartition);
    OsduAbstractFileCollection(const OsduAbstractFileCollection& other) = default;
    OsduAbstractFileCollection(OsduAbstractFileCollection&& other) noexcept = default;
    OsduAbstractFileCollection& operator=(const OsduAbstractFileCollection& rhs) = default;
    OsduAbstractFileCollection& operator=(OsduAbstractFileCollection&& rhs) noexcept = default;
    ~OsduAbstractFileCollection() override = default;

    explicit OsduAbstractFileCollection(OsduAbstractFileCollectionStruct abstractFileCollectionStruct);
    explicit OsduAbstractFileCollection(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractFileCollectionStruct fileCollectionStruct_;
};
} // namespace segysdk::metadata::osdu
