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
#include "../Utilities/JsonUtilities.h"

#include <gtest/gtest.h>
#include <string>

namespace segysdk::osduMetadata::HelperTests
{

using namespace segysdk::metadata::osdu;

std::string json_string = "{\"acl\": {\"owners\": [\"someone@company.com\"], \"viewers\": [\"someone@company.com\"]}, \"ancestry\": {\"parents\": []}, \"createTime\": \"2020-12-16T11:46:20.163Z\", \"createUser\": \"some-user@some-company-cloud.com\", \"data\": {\"Checksum\": \"d41d8cd98f00b204e9800998ecf8427e\", \"ContentSchemaKind\": \"osdu:wks:content--SeismicHistogram:1.0.0\", \"DatasetProperties\": {\"FileSourceInfo\": {\"Checksum\": \"d41d8cd98f00b204e9800998ecf8427e\", \"ChecksumAlgorithm\": \"SHA-256\", \"EncodingFormatTypeID\": \"namespace:reference-data--EncodingFormatType:application%2Fgeo%2Bjson:\", \"FileSize\": \"95463\", \"FileSource\": \"s3://default_bucket/r1/data/provided/documents/1000.witsml\", \"Name\": \"1000.witsml\", \"PreloadFileCreateDate\": \"2019-12-16T11:46:20.163Z\", \"PreloadFileCreateUser\": \"somebody@acme.org\", \"PreloadFileModifyDate\": \"2019-12-20T17:20:05.356Z\", \"PreloadFileModifyUser\": \"somebody.else@acme.org\", \"PreloadFilePath\": \"s3://staging-area/r7/raw-data/provided/documents/1000.witsml\"}}, \"Description\": \"As originally delivered by ACME.com.\", \"EncodingFormatTypeID\": \"namespace:reference-data--EncodingFormatType:text%2Fcsv:\", \"Endian\": \"BIG\", \"ExistenceKind\": \"namespace:reference-data--ExistenceKind:Prototype:\", \"ExtensionProperties\": {}, \"Name\": \"Dataset X221/15\", \"ResourceCurationStatus\": \"namespace:reference-data--ResourceCurationStatus:Created:\", \"ResourceHomeRegionID\": \"namespace:reference-data--OSDURegion:AWSEastUSA:\", \"ResourceHostRegionIDs\": [\"namespace:reference-data--OSDURegion:AWSEastUSA:\"], \"ResourceLifecycleStatus\": \"namespace:reference-data--ResourceLifecycleStatus:Loading:\", \"ResourceSecurityClassification\": \"namespace:reference-data--ResourceSecurityClassification:Public:\", \"SchemaFormatTypeID\": \"namespace:reference-data--SchemaFormatType:CWLS%20LAS3:\", \"Source\": \"Example Data Source\", \"TotalSize\": \"13245217273\"}, \"id\": \"namespace:dataset--File.SeismicHistogram:11eb0d5c-a6bd-5d74-8e32-ac8c429ee9f9\", \"kind\": \"osdu:wks:dataset--File.SeismicHistogram:1.0.0\", \"legal\": {\"legaltags\": [\"Example legaltags\"], \"otherRelevantDataCountries\": [\"US\"], \"status\": \"compliant\"}, \"meta\": [], \"modifyTime\": \"2020-12-16T11:52:24.477Z\", \"modifyUser\": \"some-user@some-company-cloud.com\", \"tags\": {\"NameOfKey\": \"String value\"}, \"version\": 1562066009929332}";

TEST(OsduMetadata, OsduReferenceStringTests)
{
    std::string osduPartition1 = "namespace";
    std::string referencePath1 = "reference-data--EncodingFormatType";
    std::string typeValue1 = "text%2Fcsv";
    OsduMetadataHelper helper1(osduPartition1);

    auto json = utilities::JsonUtilities::getJsonFromString(json_string);
    ASSERT_FALSE(json.empty());

    const auto referenceString1 = helper1.osduReferenceLookupString(referencePath1, typeValue1);
    const std::string expectedValue1 = osduPartition1 + ":" + referencePath1 + ":" + typeValue1 + ":";
    ASSERT_TRUE(referenceString1 == expectedValue1);

    ASSERT_TRUE(json.isMember("data"));
    const auto dataJson = json["data"];
    ASSERT_TRUE(dataJson.isMember("EncodingFormatTypeID"));

    const std::string expectedValue2 = dataJson["EncodingFormatTypeID"].asString();
    ASSERT_TRUE(referenceString1 == expectedValue2);

    std::string osduPartition2 = "opendes";
    OsduMetadataHelper helper2(osduPartition2);

    const auto referenceString2 = helper2.osduReferenceLookupString(referencePath1, typeValue1);
    ASSERT_FALSE(referenceString1 == referenceString2);

    const std::string osduPartition3 = "osdu";
    const std::string referencePath3 = "wks";
    const std::string typeValue3 = "content--SeismicHistogram";
    const std::string version3 = "1.0.0";

    helper2.setOsduPartition(osduPartition3);
    const auto referencePath4 = helper2.osduReferenceLookupString(referencePath3, typeValue3, version3);
    const std::string expectedValue4 = "osdu:wks:content--SeismicHistogram:1.0.0";
    ASSERT_TRUE(referencePath4 == expectedValue4);
}

TEST(OsduMetadata, OsduIdStringTests)
{
    std::string osduPartition1 = "namespace";
    std::string idClass1 = "dataset--File.SeismicHistogram";
    std::string idValue1 = "11eb0d5c-a6bd-5d74-8e32-ac8c429ee9f9";
    OsduMetadataHelper helper1(osduPartition1);

    auto json = utilities::JsonUtilities::getJsonFromString(json_string);
    ASSERT_FALSE(json.empty());

    const auto idString1 = helper1.osduStorageSystemIdString(idClass1, idValue1);
    const std::string expectedValue1 = osduPartition1 + ":" + idClass1 + ":" + idValue1;
    ASSERT_TRUE(idString1 == expectedValue1);

    ASSERT_TRUE(json.isMember("id"));

    const std::string expectedValue2 = json["id"].asString();
    ASSERT_TRUE(idString1 == expectedValue2);

    std::string osduPartition2 = "opendes";
    OsduMetadataHelper helper2(osduPartition2);

    const auto idString2 = helper2.osduStorageSystemIdString(idClass1, idValue1);
    ASSERT_FALSE(idString1 == idString2);

    const std::string osduPartition3 = "osdu";
    const std::string idClass3 = "work-product-component--SeismicBinGrid";
    const std::string idValue3 = "binGrid-09cc1a5d-d7f8-40f4-8161-bd40954f147c";

    helper2.setOsduPartition(osduPartition3);
    const auto idString = helper2.osduStorageSystemIdString(idClass3, idValue3);
    const std::string expectedValue4 = "opendes:work-product-component--SeismicBinGrid:binGrid-09cc1a5d-d7f8-40f4-8161-bd40954f147c";
    ASSERT_TRUE(idString != expectedValue4);
}

TEST(OsduMetadata, HelperOptionalValidityClassTest)
{
    const auto json = utilities::JsonUtilities::getJsonFromString(json_string);

    const std::string myPartition{"namespace"};
    OsduFileHistogram histogram(json, myPartition);

    OsduMetadataHelper helper(myPartition);
    const auto histogramStruct = std::reinterpret_pointer_cast<OsduFileHistogramStruct>(histogram.getMetaDataStruct());
    if (histogramStruct != nullptr && histogramStruct->dataStruct.has_value()) {
        ASSERT_TRUE(histogramStruct->dataStruct.value().isValid());
    }
}

class OptionTest
{
public:
    std::string getFirstName() { return (firstName_.has_value()) ? firstName_.value() : "No first name"; }
    std::string getLastName() { return (lastName_.has_value()) ? lastName_.value() : "No last name"; }
    std::string getGreeting() { return hello_ + " " + getFirstName() + " " + getLastName(); }

    OptionTest() = default;
    OptionTest(std::string firstName, std::string lastName)
      : firstName_(std::move(firstName)), lastName_(std::move(lastName)), hello_("Hello") {}

private:
    std::optional<std::string> firstName_;
    std::optional<std::string> lastName_;
    std::string hello_;
};

TEST(OsduMetadata, HelperOptionalToPointer)
{
    OptionTest sut("Mickey", "Mouse");
    ASSERT_EQ(sut.getGreeting(), "Hello Mickey Mouse");

    const auto ptr = std::make_shared<OptionTest>(sut);
    ASSERT_EQ(ptr->getGreeting(), "Hello Mickey Mouse");
}

}// namespace segysdk::osduMetadata::HelperTests
