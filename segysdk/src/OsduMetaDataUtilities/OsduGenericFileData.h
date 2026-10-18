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

#include "../OsduMetaDataUtilities/OsduAbstractDatasetStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileStruct.h"
#include "../OsduMetaDataUtilities/OsduGenericFileDataStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*

 "data": {
   "Checksum" : "d41d8cd98f00b204e9800998ecf8427e",
		"ContentSchemaKind" : "osdu:wks:content--SeismicHistogram:1.0.0",
		"DatasetProperties" : 
		{
			"FileSourceInfo" : 
			{
				"Checksum" : "d41d8cd98f00b204e9800998ecf8427e",
				"ChecksumAlgorithm" : "SHA-256",
				"EncodingFormatTypeID" : "namespace:reference-data--EncodingFormatType:application%2Fgeo%2Bjson:",
				"FileSize" : "95463",
				"FileSource" : "s3://default_bucket/r1/data/provided/documents/1000.witsml",
				"Name" : "1000.witsml",
				"PreloadFileCreateDate" : "2019-12-16T11:46:20.163Z",
				"PreloadFileCreateUser" : "somebody@acme.org",
				"PreloadFileModifyDate" : "2019-12-20T17:20:05.356Z",
				"PreloadFileModifyUser" : "somebody.else@acme.org",
				"PreloadFilePath" : "s3://staging-area/r7/raw-data/provided/documents/1000.witsml"
			}
		},
		"Description" : "As originally delivered by ACME.com.",
		"EncodingFormatTypeID" : "namespace:reference-data--EncodingFormatType:text%2Fcsv:",
		"Endian" : "BIG",
		...
        }
  
*/

class SEGYSDK_DLL_PUBLIC OsduGenericFileData final : public OsduMetadataBase
{
public:
    OsduGenericFileData(const std::string& osduPartition);
    OsduGenericFileData(const OsduGenericFileData& other);
    OsduGenericFileData(OsduGenericFileData&& other) noexcept;
    OsduGenericFileData& operator=(const OsduGenericFileData& rhs);
    OsduGenericFileData& operator=(OsduGenericFileData&& rhs) noexcept;
    ~OsduGenericFileData() override = default;

    explicit OsduGenericFileData(OsduGenericFileDataStruct fileDataStruct);
    explicit OsduGenericFileData(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    std::optional<OsduAbstractCommonResourcesStruct> getAbstractCommonResourcesStruct() const;
    std::optional<OsduAbstractDatasetStruct> getAbstractDatasetStruct() const;
    std::optional<OsduAbstractFileStruct> getAbstractFileStruct() const;
    std::optional<OsduExtensionPropertyArrayStruct> getExtensionPropertiesStruct() const;

private:
    OsduGenericFileDataStruct fileDataStruct_;
};

}
