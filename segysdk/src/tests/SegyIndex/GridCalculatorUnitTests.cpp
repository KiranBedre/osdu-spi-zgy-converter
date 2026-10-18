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
#include "../SegyIndex/IndexBuilderData.h"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class ParameterizedGridCalculatorUnitTests : public ::testing::TestWithParam<std::tuple<std::string, DataGridType>>
{

protected:
    void SetUp() override
    {
        const UnitTestDataProvider testProvider;

        UnitTestDataProvider testDataProvider;

        auto dataFilePath = dataType == DataGridType::Regular ? testDataProvider.getRegularFilesFolder() : testDataProvider.getNonRegularFilesFolder();

        ASSERT_FALSE(dataFilePath.empty());
        ASSERT_TRUE(std::filesystem::exists(dataFilePath));
        std::filesystem::path testFilePath(dataFilePath);
        testFilePath /= testFileName;

        json = testProvider.getDataAsJson(testFilePath.string(), dataType);

        ASSERT_NO_THROW();
        ASSERT_FALSE(json.empty());
    }

    std::string testFileName = std::get<0>(GetParam());
    DataGridType dataType = std::get<1>(GetParam());

    const double gcprecision = 0.001;
    const double delta = 0.00001;
    Json::Value json;

    static double
    getPositiveAngle(double degrees)
    {

        while (degrees < 0)
            degrees += 360;
        while (degrees > 360)
            degrees -= 360;

        return degrees;
    }
};

TEST_P(ParameterizedGridCalculatorUnitTests, GridCalculatorUnitTest)
{

    ASSERT_FALSE(json.empty());
    int errors = 0;
    std::vector<std::shared_ptr<IndexBuilderCollatedData>> collatedIndexDataVector = JsonUnpacker::getIndexBuilderDataFromJson(json["IndexData"], errors);
    EXPECT_EQ(errors, 0);

    Expected expected = JsonUnpacker::getExpectedResultFromJson(json, errors);
    EXPECT_EQ(errors, 0);

    GridCalculator sut(expected.getGeometry());

    for (const std::shared_ptr<IndexBuilderCollatedData>& collated : collatedIndexDataVector) {
        auto traceDataManager = collated->traceDataManager;
        EXPECT_NE(traceDataManager, nullptr);
        EXPECT_TRUE(traceDataManager->IsReaderReady());

        while (!traceDataManager->AtReaderEnd()) {
            IndexBuilderData ibd = traceDataManager->ReadNext();

            Coordinate coord = sut.GetXYLocation(ibd.InlineNo, ibd.CrosslineNo);

            EXPECT_DOUBLE_EQ(coord.X, ibd.X);
            EXPECT_DOUBLE_EQ(coord.Y, ibd.Y);

            GridCell cell = sut.GetLocation(coord);
            EXPECT_EQ(cell.i, ibd.InlineNo);
            EXPECT_EQ(cell.j, ibd.CrosslineNo);
        }
        collated->traceDataManager->CloseReader();
    }
}

INSTANTIATE_TEST_CASE_P(

    SegyIndexTests,
    ParameterizedGridCalculatorUnitTests,
    ::testing::Values(
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-0-CL-10101-10125-Count-25-Incr-90-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-0-CL-10101-10125-Count-25-Incr-270-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-30-CL-10101-10125-Count-25-Incr-120-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-30-CL-10101-10125-Count-25-Incr-300-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-90-CL-10101-10125-Count-25-Incr-0-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-90-CL-10101-10125-Count-25-Incr-180-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-120-CL-10101-10125-Count-25-Incr-30-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-120-CL-10101-10125-Count-25-Incr-210-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-180-CL-10101-10125-Count-25-Incr-90-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-180-CL-10101-10125-Count-25-Incr-270-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-210-CL-10101-10125-Count-25-Incr-120-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-210-CL-10101-10125-Count-25-Incr-300-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-270-CL-10101-10125-Count-25-Incr-0-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-270-CL-10101-10125-Count-25-Incr-180-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-300-CL-10101-10125-Count-25-Incr-30-RH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-300-CL-10101-10125-Count-25-Incr-210-LH.json", DataGridType::Regular),
        std::make_tuple("IndexTestData-IL-101-160-Count-60-Incr-205-CL-10101-10125-Count-25-Incr-115-LH.json", DataGridType::Regular),

        std::make_tuple("IndexTestData-IL-101-125-Count-25-Incr-30-CL-10101-10125-Count-25-Incr-120-RH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-25-CL-10101-10125-Count-25-Incr-115-RH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-25-CL-10101-10125-Count-25-Incr-295-LH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-115-CL-10101-10125-Count-25-Incr-25-LH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-115-CL-10101-10125-Count-25-Incr-205-RH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-205-CL-10101-10125-Count-25-Incr-115-LH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-205-CL-10101-10125-Count-25-Incr-295-RH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-295-CL-10101-10125-Count-25-Incr-25-RH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-150-Count-50-Incr-295-CL-10101-10125-Count-25-Incr-205-LH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular),
        std::make_tuple("IndexTestData-IL-101-160-Count-60-Incr-205-CL-10101-10125-Count-25-Incr-115-LH-ILSkipped-CLSkipped-CellsSkipped.json", DataGridType::NonRegular)));

} // namespace segysdk::segyindex::tests
