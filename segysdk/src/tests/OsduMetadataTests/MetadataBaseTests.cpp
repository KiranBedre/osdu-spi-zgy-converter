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

#include "../OsduMetaDataUtilities/OsduMetafileApi.h"

#include <gtest/gtest.h>
#include <optional>
#include <string>

namespace segysdk::osduMetadata::MetadataBaseTests
{

using namespace segysdk::metadata::osdu;

std::string json_string = "{\"acl\": {\"owners\": [\"someone@company.com\"], \"viewers\": [\"someone@company.com\"]}, \"ancestry\": {\"parents\": []}, \"createTime\": \"2020-12-16T11:46:20.163Z\", \"createUser\": \"some-user@some-company-cloud.com\", \"data\": {\"Checksum\": \"d41d8cd98f00b204e9800998ecf8427e\", \"ContentSchemaKind\": \"osdu:wks:content--SeismicHistogram:1.0.0\", \"DatasetProperties\": {\"FileSourceInfo\": {\"Checksum\": \"d41d8cd98f00b204e9800998ecf8427e\", \"ChecksumAlgorithm\": \"SHA-256\", \"EncodingFormatTypeID\": \"opendes:reference-data--EncodingFormatType:application%2Fgeo%2Bjson:\", \"FileSize\": \"95463\", \"FileSource\": \"s3://default_bucket/r1/data/provided/documents/1000.witsml\", \"Name\": \"1000.witsml\", \"PreloadFileCreateDate\": \"2019-12-16T11:46:20.163Z\", \"PreloadFileCreateUser\": \"somebody@acme.org\", \"PreloadFileModifyDate\": \"2019-12-20T17:20:05.356Z\", \"PreloadFileModifyUser\": \"somebody.else@acme.org\", \"PreloadFilePath\": \"s3://staging-area/r7/raw-data/provided/documents/1000.witsml\"}}, \"Description\": \"As originally delivered by ACME.com.\", \"EncodingFormatTypeID\": \"opendes:reference-data--EncodingFormatType:text%2Fcsv:\", \"Endian\": \"BIG\", \"ExistenceKind\": \"opendes:reference-data--ExistenceKind:Prototype:\", \"ExtensionProperties\": {}, \"Name\": \"Dataset X221/15\", \"ResourceCurationStatus\": \"opendes:reference-data--ResourceCurationStatus:Created:\", \"ResourceHomeRegionID\": \"opendes:reference-data--OSDURegion:AWSEastUSA:\", \"ResourceHostRegionIDs\": [\"opendes:reference-data--OSDURegion:AWSEastUSA:\"], \"ResourceLifecycleStatus\": \"opendes:reference-data--ResourceLifecycleStatus:Loading:\", \"ResourceSecurityClassification\": \"opendes:reference-data--ResourceSecurityClassification:Public:\", \"SchemaFormatTypeID\": \"opendes:reference-data--SchemaFormatType:CWLS%20LAS3:\", \"Source\": \"Example Data Source\", \"TotalSize\": \"13245217273\"}, \"id\": \"opendes:dataset--File.SeismicHistogram:11eb0d5c-a6bd-5d74-8e32-ac8c429ee9f9\", \"kind\": \"osdu:wks:dataset--File.SeismicHistogram:1.0.0\", \"legal\": {\"legaltags\": [\"Example legaltags\"], \"otherRelevantDataCountries\": [\"US\"], \"status\": \"compliant\"}, \"meta\": [], \"modifyTime\": \"2020-12-16T11:52:24.477Z\", \"modifyUser\": \"some-user@some-company-cloud.com\", \"tags\": {\"NameOfKey\": \"String value\"}, \"version\": 1562066009929332}";

TEST(OsduMetadata, OptionalValidityTest)
{
    const std::string myPartition{"myOsduPartition"};
    const std::string path{"reference-data--EncodingFormatType"};
    const std::string value{"Json"};

    OsduMetadataHelper helper(myPartition);

    OsduAbstractDatasetStruct data(myPartition);
    data.encodingFormatTypeID = helper.osduReferenceLookupString(path, value);

    ASSERT_TRUE(helper.isValidOptionalObject(data.encodingFormatTypeID));
}

} // namespace segysdk::osduMetadata::MetadataBaseTests
