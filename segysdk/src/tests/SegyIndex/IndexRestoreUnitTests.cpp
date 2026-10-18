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

#include "JsonUnpacker.h"
#include "UnitTestDataProvider.h"

#include "../SegyIndex/SegyIndexImpl.h"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class ParameterizedIndexRestoreUnitTests : public ::testing::TestWithParam<std::string>
{

protected:
    void SetUp() override
    {
        const UnitTestDataProvider testProvider;

        UnitTestDataProvider testDataProvider;

        auto regularPath = testDataProvider.getRegularFilesFolder();

        ASSERT_FALSE(regularPath.empty());
        ASSERT_TRUE(std::filesystem::exists(regularPath));
        std::filesystem::path testFilePath(regularPath);
        testFilePath /= testFileName;

        json = testProvider.getDataAsJson(testFilePath.string(), DataGridType::Regular);

        ASSERT_NO_THROW();
        ASSERT_FALSE(json.empty());
    }

    std::string testFileName = GetParam();

    const double anglePrecision = 0.00001;
    const double distancePrecision = 0.001;

    Json::Value json;

    static double
    getPositiveAngle(double degrees)
    {

        while (degrees < 0)
            degrees += 360;
        while (degrees >= 360)
            degrees -= 360;

        return degrees;
    }
};

TEST_P(ParameterizedIndexRestoreUnitTests, BufferOperationsUnitTest_PositiveValues)
{
    static constexpr uint16_t const fileNo = static_cast<uint16_t>(0x0102);
    static constexpr int32_t const inlineNo = static_cast<int32_t>(0x03040506);
    static constexpr int32_t const crosslineNo = static_cast<int32_t>(0x07080910);
    static constexpr uint64_t const fileOffsetNo = static_cast<uint64_t>(0x1112131415161718);

    static constexpr size_t const expectedBufferSize = sizeof(fileNo) + sizeof(inlineNo) + sizeof(crosslineNo) + sizeof(fileOffsetNo);

    std::vector<unsigned char> buffer;
    buffer.reserve(expectedBufferSize);

    index_storage::SegyIndexStorage::append_to_buffer<uint16_t>(buffer, fileNo);
    index_storage::SegyIndexStorage::append_to_buffer<int32_t>(buffer, inlineNo);
    index_storage::SegyIndexStorage::append_to_buffer<int32_t>(buffer, crosslineNo);
    index_storage::SegyIndexStorage::append_to_buffer<uint64_t>(buffer, fileOffsetNo);

    ASSERT_EQ(buffer.size(), expectedBufferSize);

    size_t bufferOffset = 0U;
    const uint16_t fileNoTest = index_storage::SegyIndexStorage::getUShortFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(uint16_t);
    const int inlineNoTest = index_storage::SegyIndexStorage::getIntFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(int32_t);
    const int crosslineNoTest = index_storage::SegyIndexStorage::getIntFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(int32_t);
    const size_t fileOffsetNoTest = index_storage::SegyIndexStorage::getSizeFromBuffer(buffer.data(), bufferOffset);

    ASSERT_EQ(fileNoTest, fileNo);
    ASSERT_EQ(inlineNoTest, inlineNo);
    ASSERT_EQ(crosslineNoTest, crosslineNo);
    ASSERT_EQ(fileOffsetNoTest, fileOffsetNo);
}

TEST_P(ParameterizedIndexRestoreUnitTests, BufferOperationsUnitTest_NegativeValues)
{
    static constexpr uint16_t const fileNo = static_cast<uint16_t>(0x0102);
    static constexpr int32_t const inlineNo = -120;
    static constexpr int32_t const crosslineNo = -900;
    static constexpr uint64_t const fileOffsetNo = static_cast<uint64_t>(0x1112131415161718);

    static constexpr size_t const expectedBufferSize = sizeof(fileNo) + sizeof(inlineNo) + sizeof(crosslineNo) + sizeof(fileOffsetNo);

    std::vector<unsigned char> buffer;
    buffer.reserve(expectedBufferSize);

    index_storage::SegyIndexStorage::append_to_buffer<uint16_t>(buffer, fileNo);
    index_storage::SegyIndexStorage::append_to_buffer<int32_t>(buffer, inlineNo);
    index_storage::SegyIndexStorage::append_to_buffer<int32_t>(buffer, crosslineNo);
    index_storage::SegyIndexStorage::append_to_buffer<uint64_t>(buffer, fileOffsetNo);

    ASSERT_EQ(buffer.size(), expectedBufferSize);

    size_t bufferOffset = 0U;
    const uint16_t fileNoTest = index_storage::SegyIndexStorage::getUShortFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(uint16_t);
    const int inlineNoTest = index_storage::SegyIndexStorage::getIntFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(int32_t);
    const int crosslineNoTest = index_storage::SegyIndexStorage::getIntFromBuffer(buffer.data(), bufferOffset);
    bufferOffset += sizeof(int32_t);
    const size_t fileOffsetNoTest = index_storage::SegyIndexStorage::getSizeFromBuffer(buffer.data(), bufferOffset);

    ASSERT_EQ(fileNoTest, fileNo);
    ASSERT_EQ(inlineNoTest, inlineNo);
    ASSERT_EQ(crosslineNoTest, crosslineNo);
    ASSERT_EQ(fileOffsetNoTest, fileOffsetNo);
}

TEST_P(ParameterizedIndexRestoreUnitTests, GridCalculatorUnitTest)
{

    ASSERT_FALSE(json.empty());
    int errors = 0;
    std::vector<std::shared_ptr<IndexBuilderCollatedData>> collatedIndexDataVector = JsonUnpacker::getIndexBuilderDataFromJson(json["IndexData"], errors);
    EXPECT_EQ(errors, 0);

    Expected expected = JsonUnpacker::getExpectedResultFromJson(json, errors);
    EXPECT_EQ(errors, 0);

    std::string testFile = "Dummy.sgy";
    std::vector segyFileNames{testFile};

    fileio::FileReaderOptions readerOptions(SDCredentials::none(), CacheSizeOption::none());
    SegyIndexImpl index(/*segyFilesVector*/ {}, segyFileNames, readerOptions);

    ASSERT_TRUE(index.ProcessIndexBuildingData(collatedIndexDataVector,
                                               segyio::VerticalDomain::Time,
                                               segyio::VerticalUOM::seconds,
                                               static_cast<size_t>(expected.getParameter().SegySampleSize)));

    index.Save();

    EXPECT_NEAR(getPositiveAngle(index.getGeometry3D().dataGrid.JVectorRotationAngle), getPositiveAngle(expected.getGeometry().dataGrid.JVectorRotationAngle), anglePrecision);
    EXPECT_NEAR(index.getGeometry3D().dataGrid.JVectorUnitDistance, expected.getGeometry().dataGrid.JVectorUnitDistance, distancePrecision);

    EXPECT_NEAR(getPositiveAngle(index.getGeometry3D().dataGrid.IVectorRotationAngle), getPositiveAngle(expected.getGeometry().dataGrid.IVectorRotationAngle), anglePrecision);
    EXPECT_NEAR(index.getGeometry3D().dataGrid.IVectorUnitDistance, expected.getGeometry().dataGrid.IVectorUnitDistance, distancePrecision);
}

INSTANTIATE_TEST_SUITE_P(

    SegyIndexTests,
    ParameterizedIndexRestoreUnitTests,
    ::testing::Values(
        "IndexTestData-IL-101-125-Count-25-Incr-0-CL-10101-10125-Count-25-Incr-90-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-0-CL-10101-10125-Count-25-Incr-270-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-30-CL-10101-10125-Count-25-Incr-120-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-30-CL-10101-10125-Count-25-Incr-300-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-90-CL-10101-10125-Count-25-Incr-0-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-90-CL-10101-10125-Count-25-Incr-180-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-120-CL-10101-10125-Count-25-Incr-30-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-120-CL-10101-10125-Count-25-Incr-210-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-180-CL-10101-10125-Count-25-Incr-90-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-180-CL-10101-10125-Count-25-Incr-270-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-210-CL-10101-10125-Count-25-Incr-120-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-210-CL-10101-10125-Count-25-Incr-300-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-270-CL-10101-10125-Count-25-Incr-0-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-270-CL-10101-10125-Count-25-Incr-180-LH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-300-CL-10101-10125-Count-25-Incr-30-RH.json",
        "IndexTestData-IL-101-125-Count-25-Incr-300-CL-10101-10125-Count-25-Incr-210-LH.json",
        "IndexTestData-IL-101-160-Count-60-Incr-205-CL-10101-10125-Count-25-Incr-115-LH.json"));

} // namespace segysdk::segyindex::tests
