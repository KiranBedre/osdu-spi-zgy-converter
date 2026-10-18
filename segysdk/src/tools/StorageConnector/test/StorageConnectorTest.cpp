/*
Copyright 2025 Schlumberger

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

#include "tools/StorageConnector/StorageConnector.h"

#include "Utilities/JsonParser.h"
#include "tools/Error/Error.h"
#include "tools/StorageConnector/test/StorageClient.h"

#include <gtest/gtest.h>

namespace tool::tests
{

static constexpr auto workProductId = "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123";
static constexpr auto seismicTraceDataId123 = "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123";
static constexpr auto seismicTraceDataId456 = "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:456";
static constexpr auto fileCollectionSegy123 = "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:123";
static constexpr auto fileCollectionSegy456 = "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:456";
static constexpr auto fileCollectionSegy789 = "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:789";
static constexpr auto fileCollectionSegy012 = "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:012";

class StorageConnectorTest : public ::testing::Test
{
public:
    Params params_ = Params{}
                         .withCorrelationId("ba3f452f-3998-491b-87d6-d413ded46242")
                         .withWorkProductId(workProductId)
                         .withFileCollectionId("sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:123")
                         .withSliceType("inline");
};

static auto
makeStorage(StorageRecords&& records = {})
{
    return StorageConnector{std::make_unique<tests::StorageClient>(std::move(records))};
}

TEST_F(StorageConnectorTest, addFileImagePngRecord)
{
    const auto& result = segysdk::parseFromString(
                             makeStorage().addFileImagePngRecord(
                                 params_, "sd://sandbox-weu-des-prod-testing-e/yz20250306/test.segy.thumbnail-227x2001-inline.png", 1298))
                             .toStyledString();
    const auto& sample = segysdk::parseFromString(R"(
            {
              "data": {
                "DatasetProperties": {
                  "FileSourceInfo": { "FileSize": 1298, "FileSource": "sd://sandbox-weu-des-prod-testing-e/yz20250306/test.segy.thumbnail-227x2001-inline.png" }
                },
                "IsExtendedLoad": false
              },
              "kind": "osdu:wks:dataset--File.Image.PNG:1.1.0",
              "tags": { "dataflowId": "ba3f452f-3998-491b-87d6-d413ded46242", "sliceType": "inline" }
            }
        )")
                             .toStyledString();
    EXPECT_EQ(result, sample) << result;
}

TEST_F(StorageConnectorTest, addFileImagePngArtefacts)
{
    params_.withSeismicTraceDataId(seismicTraceDataId123);
    const auto& result = segysdk::parseFromString(
                             makeStorage().addFileImagePngArtefacts(
                                 params_,
                                 {
                                     "sandbox-weu-des-prod-testing-e:dataset--File.Image.PNG:456",
                                     "sandbox-weu-des-prod-testing-e:dataset--File.Image.PNG:789",
                                 }))
                             .toStyledString();
    const auto& sample = segysdk::parseFromString(R"(
        {
          "data": {
            "Artefacts": [
              {
                "ResourceID": "sandbox-weu-des-prod-testing-e:dataset--File.Image.PNG:456:",
                "ResourceKind": "osdu:wks:dataset--File.Image.PNG:1.1.0",
                "RoleID": "sandbox-weu-des-prod-testing-e:reference-data--ArtefactRole:EmbeddedImage:"
              },
              {
                "ResourceID": "sandbox-weu-des-prod-testing-e:dataset--File.Image.PNG:789:",
                "ResourceKind": "osdu:wks:dataset--File.Image.PNG:1.1.0",
                "RoleID": "sandbox-weu-des-prod-testing-e:reference-data--ArtefactRole:EmbeddedImage:"
              }
            ]
          },
          "recordId": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123"
        }
    )")
                             .toStyledString();
    EXPECT_EQ(result, sample) << result;
}

static auto
getTraceDataWithSingleFileCollection()
{
    return R"(
         {
           "data": { "Datasets": ["sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:123:"] },
           "id": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123",
           "kind": "osdu:wks:work-product-component--SeismicTraceData:1.4.0"
         }
    )";
}

static auto
getSegyFileCollection()
{
    return R"(
        {
          "data": {
            "DatasetProperties": { "FileSourceInfos": [{ "FileSource": "sd://sandbox-weu-des-prod-testing-e/yz20250306/test.segy" }] }
          },
          "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",
          "legal": {
            "legaltags": ["sandbox-weu-des-prod-testing-e-referencedata-legal"],
            "otherRelevantDataCountries": ["US"],
            "status": "compliant"
          }
       }
    )";
}

TEST_F(StorageConnectorTest, getFileCollections1WorkProduct1SeismicTraceData)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
            {seismicTraceDataId123, getTraceDataWithSingleFileCollection()},
            {fileCollectionSegy123, getSegyFileCollection()},
        });
    const auto [descriptors] = storage.getContext(params_);
    ASSERT_EQ(descriptors.size(), 1);
    EXPECT_EQ(descriptors.at(0).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(0).fileCollectionDescriptor_.id_, fileCollectionSegy123);
}

static auto
getTraceDataWithDoubleFileCollection()
{
    return R"(
        {
          "data": {
            "Datasets": [
              "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:123:",
              "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:456:"
            ]
          },
          "id": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123",
          "kind": "osdu:wks:work-product-component--SeismicTraceData:1.4.0"
        }
    )";
}

TEST_F(StorageConnectorTest, getFileCollections1WorkProduct2SeismicTraceData)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
            {seismicTraceDataId123, getTraceDataWithDoubleFileCollection()},
            {fileCollectionSegy123, getSegyFileCollection()},
            {fileCollectionSegy456, getSegyFileCollection()},
        });
    const auto [descriptors] = storage.getContext(params_);
    ASSERT_EQ(descriptors.size(), 2);
    EXPECT_EQ(descriptors.at(0).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(0).fileCollectionDescriptor_.id_, fileCollectionSegy123);
    EXPECT_EQ(descriptors.at(1).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(1).fileCollectionDescriptor_.id_, fileCollectionSegy456);
}

TEST_F(StorageConnectorTest, getFileCollections2WorkProduct2SeismicTraceData)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:456:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
            {seismicTraceDataId123, getTraceDataWithDoubleFileCollection()},
            {seismicTraceDataId456, R"(
                {
                  "data": {
                    "Datasets": [
                      "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:789:",
                      "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:789:",
                      "sandbox-weu-des-prod-testing-e:dataset--FileCollection.SEGY:012:"
                    ]
                  },
                  "id": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:456",
                  "kind": "osdu:wks:work-product-component--SeismicTraceData:1.4.0"
                }
             )"},
            {fileCollectionSegy123, getSegyFileCollection()},
            {fileCollectionSegy456, getSegyFileCollection()},
            {fileCollectionSegy789, getSegyFileCollection()},
            {fileCollectionSegy012, getSegyFileCollection()},
        });
    const auto [descriptors] = storage.getContext(params_);
    ASSERT_EQ(descriptors.size(), 5);
    EXPECT_EQ(descriptors.at(0).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(0).fileCollectionDescriptor_.id_, fileCollectionSegy123);
    EXPECT_EQ(descriptors.at(1).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(1).fileCollectionDescriptor_.id_, fileCollectionSegy456);
    EXPECT_EQ(descriptors.at(2).id_, seismicTraceDataId456);
    EXPECT_EQ(descriptors.at(2).fileCollectionDescriptor_.id_, fileCollectionSegy789);
    EXPECT_EQ(descriptors.at(3).id_, seismicTraceDataId456);
    EXPECT_EQ(descriptors.at(3).fileCollectionDescriptor_.id_, fileCollectionSegy789);
    EXPECT_EQ(descriptors.at(4).id_, seismicTraceDataId456);
    EXPECT_EQ(descriptors.at(4).fileCollectionDescriptor_.id_, fileCollectionSegy012);
}

TEST_F(StorageConnectorTest, getFileCollections0WorkProduct)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
            {seismicTraceDataId123, getTraceDataWithSingleFileCollection()},
        });
    EXPECT_THROW(storage.getContext(params_), Error);
}

TEST_F(StorageConnectorTest, getFileCollections1WorkProduct0SeismicTraceData)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
        });
    EXPECT_THROW(storage.getContext(params_), Error);
}

TEST_F(StorageConnectorTest, getFileCollections1WorkProduct0FileCollection)
{
    const auto& storage = makeStorage(
        {
            {workProductId, R"(
                 {
                   "data": {
                     "Components": [
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123:",
                       "sandbox-weu-des-prod-testing-e:work-product-component--SeismicBinGrid:123:"
                     ]
                   },
                   "id": "sandbox-weu-des-prod-testing-e:work-product--WorkProduct:123",
                   "kind": "osdu:wks:work-product--WorkProduct:1.0.0"
                 }
             )"},
            {seismicTraceDataId123, R"(
                 {
                   "data": { "Datasets": [] },
                   "id": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:123",
                   "kind": "osdu:wks:work-product-component--SeismicTraceData:1.4.0"
                 }
             )"},
        });
    EXPECT_THROW(storage.getContext(params_), Error);
}

TEST_F(StorageConnectorTest, getFileCollections1SeismicTraceData1FileCollection)
{
    params_.withSeismicTraceDataId(seismicTraceDataId123);
    const auto& storage = makeStorage(
        {
            {seismicTraceDataId123, getTraceDataWithSingleFileCollection()},
            {fileCollectionSegy123, getSegyFileCollection()},
        });
    const auto [descriptors] = storage.getContext(params_);
    ASSERT_EQ(descriptors.size(), 1);
    EXPECT_EQ(descriptors.at(0).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(0).fileCollectionDescriptor_.id_, fileCollectionSegy123);
}

TEST_F(StorageConnectorTest, getFileCollections1SeismicTraceData2FileCollection)
{
    params_.withSeismicTraceDataId(seismicTraceDataId123);
    const auto& storage = makeStorage(
        {
            {seismicTraceDataId123, getTraceDataWithDoubleFileCollection()},
            {fileCollectionSegy123, getSegyFileCollection()},
            {fileCollectionSegy456, getSegyFileCollection()},
        });
    const auto [descriptors] = storage.getContext(params_);
    ASSERT_EQ(descriptors.size(), 2);
    EXPECT_EQ(descriptors.at(0).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(0).fileCollectionDescriptor_.id_, fileCollectionSegy123);
    EXPECT_EQ(descriptors.at(1).id_, seismicTraceDataId123);
    EXPECT_EQ(descriptors.at(1).fileCollectionDescriptor_.id_, fileCollectionSegy456);
}

} // namespace tool::tests
