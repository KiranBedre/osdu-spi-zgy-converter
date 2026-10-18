/*
Copyright 2021-2022 Schlumberger

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

#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"
#include "../tools/SegyToZgy/library/osdu/OsduFileCollectionOpenZGY100.h"

#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/osdu/GeneratedOutputFileName.h"
#include "../Utilities/osdu/OsduFieldNameLiterals.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduKind.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../Utilities/osdu/OsduObjectReference.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../Utilities/osdu/OsduWorkProduct100.h"

#include <gtest/gtest.h>

#include <filesystem>

static std::string
testFilePath(const std::string& fileName)
{
    const std::string filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    return (std::filesystem::path{filePath} / "OSDU" / fileName).string();
}

TEST(OsduRecordsTests, test_object)
{
    const auto inputFilename = testFilePath("WorkProduct.1.0.0.json");

    const segysdk::utilities::osdu::OsduObject sut(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    EXPECT_EQ(sut.ns(), "namespace");
    EXPECT_EQ(sut.id(), "namespace:work-product--WorkProduct:43809ad8-f097-58b0-ba61-343d848a6047");
    EXPECT_EQ(sut.version(), "1562066009929332");
    EXPECT_EQ(sut.kind().kind(), "osdu:wks:work-product--WorkProduct:1.0.0");
}

TEST(OsduRecordsTests, test_workproduct)
{
    const auto inputFilename = testFilePath("WorkProduct.1.0.0.json");

    const segysdk::utilities::osdu::OsduWorkProduct100 sut(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    const auto components = sut.componentIds();
    EXPECT_EQ(components.size(), 1);
    EXPECT_EQ(components[0], "namespace:work-product-component--AnyWorkProductComponent:AnyWorkProductComponent-911bb71f-06ab-4deb-8e68-b8c9229dc76b:");
}

TEST(OsduRecordsTests, test_filecollection_segy_source_files)
{
    const auto inputFilename = testFilePath("FileCollection.SEGY.1.0.0.json");

    const segysdk::utilities::osdu::OsduFileCollectionSegy100 sut(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    const auto files = sut.segyFileNames();
    EXPECT_EQ(files.size(), 2);
    EXPECT_EQ(files[0], "s3://default_bucket/opendes/data/vds-dataset/vds-file-1");
    EXPECT_EQ(files[1], "s3://default_bucket/opendes/data/vds-dataset/vds-file-2");
}

TEST(OsduRecordsTests, test_zgy_file_name)
{
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sgy", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.SgY", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sgy", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.segy", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sEgY", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.segy", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.someext", "").fileName, "test.someext.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.someext", "suffix").fileName, "test.someext.suffix.zgy");
}

TEST(OsduRecordsTests, test_filecollection_segy_target_file_name)
{
    const auto inputFilenameNoSgyExt = testFilePath("FileCollection.SEGY.1.0.0.json");
    const auto inputFilenameSgyExt = testFilePath("FileCollection.SEGY.1.0.0_sgy_extension.json");

    const segysdk::utilities::osdu::OsduFileCollectionSegy100 sut(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilenameSgyExt));
    const auto zgy = sut.desiredZgyFileName("");

    EXPECT_EQ(zgy, "s3://default_bucket/opendes/data/segy-dataset/segy-file-1.zgy");
    const auto zgyCustomSuffix = sut.desiredZgyFileName("customsuffix");
    EXPECT_EQ(zgyCustomSuffix, "s3://default_bucket/opendes/data/segy-dataset/segy-file-1.customsuffix.zgy");
}

TEST(OsduRecordsTests, test_kind)
{
    const segysdk::utilities::osdu::OsduKind sut("osdu:wks:work-product-component--SeismicTraceData:1.0.0");
    EXPECT_EQ(sut.fullTypeName(), "osdu:wks:work-product-component--SeismicTraceData");
    EXPECT_EQ(sut.shortTypeName(), "work-product-component--SeismicTraceData");
}

TEST(OsduRecordsTests, test_kind_changeType)
{
    const segysdk::utilities::osdu::OsduKind original("osdu:wks:work-product-component--SeismicTraceData:1.0.0");
    const auto sut = original.changeType("dataset--FileCollection.Slb.OpenZGY:1.0.0");
    EXPECT_EQ(sut.kind(), "osdu:wks:dataset--FileCollection.Slb.OpenZGY:1.0.0");
    EXPECT_EQ(sut.fullTypeName(), "osdu:wks:dataset--FileCollection.Slb.OpenZGY");
    EXPECT_EQ(sut.shortTypeName(), "dataset--FileCollection.Slb.OpenZGY");
}

TEST(OsduRecordsTests, test_seismic_trace_data)
{
    const auto inputFilename = testFilePath("SeismicTraceData.1.0.0.json");

    const auto sut = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    auto const binGridId = sut.binGridId();
    EXPECT_EQ(sut.binGridId(), "namespace:work-product-component--SeismicBinGrid:SeismicBinGrid-911bb71f-06ab-4deb-8e68-b8c9229dc76b:");
}

TEST(OsduRecordsTests, test_seismic_trace_data_setOpenZGYDatasetId)
{
    const auto inputFilename = testFilePath("SeismicTraceData.1.0.0.json");

    auto sut = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    ASSERT_EQ(sut.artefactIds().size(), 1);
    ASSERT_EQ(sut.artefactIds()[0], "namespace:dataset--AnyDataset:SomeUniqueAnyDatasetID:");

    sut.addCompletedOpenZGYDataset("new-id", segysdk::utilities::osdu::OsduKind(""));
    EXPECT_EQ(sut.datasetIds().size(), 1);
    EXPECT_EQ(sut.datasetIds()[0], "namespace:dataset--AnyDataset:SomeUniqueAnyDatasetID:");

    EXPECT_EQ(sut.artefactIds().size(), 2);
    EXPECT_EQ(sut.artefactIds()[0], "namespace:dataset--AnyDataset:SomeUniqueAnyDatasetID:");
    EXPECT_EQ(sut.artefactIds()[1], "new-id:");
}

TEST(OsduRecordsTests, test_seismic_trace_data_setOpenZGYDatasetId_minimal)
{
    const auto inputFilename = testFilePath("SeismicTraceData.1.0.0_minimal.json");

    auto sut = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename));
    ASSERT_EQ(sut.artefactIds().size(), 0);

    sut.addCompletedOpenZGYDataset("new-id", segysdk::utilities::osdu::OsduKind(""));
    EXPECT_EQ(sut.datasetIds().size(), 0);

    EXPECT_EQ(sut.artefactIds().size(), 1);
    EXPECT_EQ(sut.artefactIds()[0], "new-id:");
}

TEST(OsduRecordsTests, test_object_reference)
{
    const auto latestVersion = segysdk::utilities::osdu::OsduObjectReference("opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925:");
    EXPECT_EQ(latestVersion.objectId(), "opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925");
    EXPECT_TRUE(latestVersion.isLatestVersion());
    EXPECT_EQ(latestVersion.specifiedVersion(), segysdk::utilities::osdu::OsduObjectReference::noVersion);

    const auto specificVersion = segysdk::utilities::osdu::OsduObjectReference("opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925:1614268858461272");
    EXPECT_EQ(specificVersion.objectId(), "opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925");
    EXPECT_FALSE(specificVersion.isLatestVersion());
    EXPECT_EQ(specificVersion.specifiedVersion(), 1614268858461272);

    const auto workaround = segysdk::utilities::osdu::OsduObjectReference("opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925");
    EXPECT_EQ(workaround.objectId(), "opendes:dataset--FileCollection.SEGY:60e1cb1a-adef-4113-85fb-143d48eea925");
    EXPECT_TRUE(workaround.isLatestVersion());
    EXPECT_EQ(workaround.specifiedVersion(), segysdk::utilities::osdu::OsduObjectReference::noVersion);
}

TEST(OsduRecordsTests, test_dimensionality)
{
    using namespace segysdk::utilities::osdu;

    const auto inputFilename = testFilePath("SeismicTraceData.1.0.0.json");

    auto jsonData = segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename);
    OsduSeismicTraceData100 sut(jsonData);

    EXPECT_FALSE(sut.isDimensionalityType2D());

    jsonData[OsduFieldNameLiterals::Data][OsduSeismicTraceDataLiterals::SeismicTraceDataDimensionalityTypeID] = "opendes:reference-data--SeismicTraceDataDimensionalityType:2D:";
    OsduSeismicTraceData100 sut2(jsonData);

    EXPECT_TRUE(sut2.isDimensionalityType2D());
}

class FileCollectionOpenZGYTests : public ::testing::Test
{
protected:
    std::unique_ptr<OsduFileCollectionOpenZGY100> sut;

    void SetUp() override
    {
        const auto source = segysdk::utilities::osdu::OsduFileCollectionSegy100(segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(testFilePath("FileCollection.SEGY.1.0.0_sgy_extension.json")));
        const auto fp = OpenZGYFileProperties::testBackdoor("sd://tenant/project/dummy_name.zgy", 123456);
        sut = std::make_unique<OsduFileCollectionOpenZGY100>(source, fp);
    }
};

TEST_F(FileCollectionOpenZGYTests, test_filecollection_openzgy)
{
    EXPECT_EQ(sut->kind().kind(), "osdu:wks:dataset--FileCollection.Slb.OpenZGY:1.0.0");
    EXPECT_EQ(sut->zgyFileName(), "sd://tenant/project/dummy_name.zgy");

    EXPECT_EQ(sut->zgyFileSize(), "123456");
    EXPECT_EQ(sut->totalFileSize(), "123456");
}

TEST_F(FileCollectionOpenZGYTests, legalTagsSameAsTheSourceSEGY)
{
    const auto legalTags = sut->legalTags();
    ASSERT_EQ(legalTags.size(), 1);
    EXPECT_EQ(legalTags[0], "Example legaltags");
}

TEST_F(FileCollectionOpenZGYTests, parentIdsMatchSourceSEGYId)
{
    const auto parentIds = sut->parentIds();
    ASSERT_EQ(parentIds.size(), 1);
    EXPECT_EQ(parentIds[0], "namespace:dataset--FileCollection.SEGY:b47f793a-dc9c-5d11-8d59-6cc611a98926:1562066009929332");
}
