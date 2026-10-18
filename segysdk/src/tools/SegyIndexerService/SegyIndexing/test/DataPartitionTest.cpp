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

#include "tools/SegyIndexerService/SegyIndexing/DataPartition.h"

#include "Utilities/JsonParser.h"

#include <gtest/gtest.h>

namespace indexer::tests
{

TEST(DataPartitionTests, fromId)
{
    EXPECT_EQ(
        getPartitionFromSeismicTraceData(segysdk::parseFromString(R"(
            {
              "data": {
                "SpatialArea": {
                  "AsIngestedCoordinates": {
                    "CoordinateReferenceSystemID": "CoordinateReferenceSystemID::"
                  },
                  "SpatialGeometryTypeID": "CoordinateReferenceSystemID::"
                }
              },
              "id": "sandbox-weu-des-prod-testing-e:work-product-component--SeismicTraceData:a3653f5f8eae4e7db969a148598dfc06"
            }
        )")),
        "sandbox-weu-des-prod-testing-e");
}

TEST(DataPartitionTests, fromSpatialGeometryTypeId)
{
    EXPECT_EQ(
        getPartitionFromSeismicTraceData(segysdk::parseFromString(R"(
            {
              "data": {
                "SpatialArea": {
                  "AsIngestedCoordinates": {
                    "CoordinateReferenceSystemID": "CoordinateReferenceSystemID:::"
                  },
                  "SpatialGeometryTypeID": "SpatialGeometryTypeID::"
                }
              }
            }
        )")),
        "SpatialGeometryTypeID");
}

TEST(DataPartitionTests, fromCoordinateReferenceSystemId)
{
    EXPECT_EQ(
        getPartitionFromSeismicTraceData(segysdk::parseFromString(R"(
            {
              "data": {
                "SpatialArea": {
                  "AsIngestedCoordinates": {
                    "CoordinateReferenceSystemID": "CoordinateReferenceSystemID::"
                  }
                }
              }
            }
        )")),
        "CoordinateReferenceSystemID");
}

TEST(DataPartitionTests, dummyPartition)
{
    EXPECT_EQ(
        getPartitionFromSeismicTraceData(segysdk::parseFromString(R"(
            {
              "data": {
                "SpatialArea": {
                  "AsIngestedCoordinates": {
                    "Key": "Value::"
                  }
                }
              }
            }
        )")),
        "dummyPartition");
}

} // namespace indexer::tests
