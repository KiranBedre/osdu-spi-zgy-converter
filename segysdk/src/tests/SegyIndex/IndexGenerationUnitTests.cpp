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

#include "../SegyIndex/GridCalculator.h"
#include "../SegyIndex/SegyIndexImpl.h"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class ParameterizedIndexGeneratingUnitTests : public ::testing::TestWithParam<std::string>
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

TEST_P(ParameterizedIndexGeneratingUnitTests, IndexGenerationUnitTest)
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

    EXPECT_EQ(index.getGeometry3D().IVectorExtent, expected.getGeometry().IVectorExtent);
    EXPECT_EQ(index.getGeometry3D().JVectorExtent, expected.getGeometry().JVectorExtent);
    EXPECT_TRUE(index.getGeometry3D().sampleExtent.almostEqual(expected.getGeometry().sampleExtent, 0.0001));
    EXPECT_TRUE(index.getGeometry3D().dataGrid.gridCell_11.almostEqual(expected.getGeometry().dataGrid.gridCell_11, 0.0001));
    EXPECT_TRUE(index.getGeometry3D().dataGrid.gridCell_1N.almostEqual(expected.getGeometry().dataGrid.gridCell_1N, 0.0001));
    EXPECT_TRUE(index.getGeometry3D().dataGrid.gridCell_N1.almostEqual(expected.getGeometry().dataGrid.gridCell_N1, 0.0001));
    EXPECT_TRUE(index.getGeometry3D().dataGrid.gridCell_NN.almostEqual(expected.getGeometry().dataGrid.gridCell_NN, 0.0001));
    EXPECT_TRUE(index.getGeometry3D().dataGrid.almostEqual(expected.getGeometry().dataGrid, 0.00001, 0.01));
    EXPECT_EQ(index.isInlineOrdered(), expected.getParameter().CubeIsInlineSorted);
    EXPECT_TRUE(index.getGeometry3D().almostEqual(expected.getGeometry(), 0.00001, 0.01));

    EXPECT_NEAR(getPositiveAngle(index.getGeometry3D().dataGrid.JVectorRotationAngle), getPositiveAngle(expected.getGeometry().dataGrid.JVectorRotationAngle), anglePrecision);
    EXPECT_NEAR(index.getGeometry3D().dataGrid.JVectorUnitDistance, expected.getGeometry().dataGrid.JVectorUnitDistance, distancePrecision);

    EXPECT_NEAR(getPositiveAngle(index.getGeometry3D().dataGrid.IVectorRotationAngle), getPositiveAngle(expected.getGeometry().dataGrid.IVectorRotationAngle), anglePrecision);
    EXPECT_NEAR(index.getGeometry3D().dataGrid.IVectorUnitDistance, expected.getGeometry().dataGrid.IVectorUnitDistance, distancePrecision);
}

INSTANTIATE_TEST_CASE_P(

    SegyIndexTests,
    ParameterizedIndexGeneratingUnitTests,
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
