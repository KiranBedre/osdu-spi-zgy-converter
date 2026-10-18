/*
Copyright 2024 Schlumberger

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
#include "../SegyIndex/SegyIndexImpl.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class Index2DGeneratingUnitTests : public ::testing::TestWithParam<std::string>
{
protected:
    void SetUp() override
    {
        testFileName_ = GetParam();
        const std::string filePath = getTestDataFilePath(testFileName_);
        ASSERT_TRUE(std::filesystem::exists(filePath));

        json_ = utilities::JsonUtilities::GetJsonObjectFromFile(filePath);

        ASSERT_NO_THROW();
        ASSERT_FALSE(json_.empty());
    }

    std::string getTestDataFilePath(std::string fileName) {
        std::filesystem::path testdataDir = utilities::Environment::instance().SegyLocalTestDataPath();
        if (testdataDir.empty()) {
            std::cerr << "Missing environment variable " << testdataDir << std::endl;
            throw std::runtime_error("Environment variable SEGY_LOCALTESTDATA_PATH has not been set.");
        }

        testdataDir.append(dataDir2D_);
        
        if (not std::filesystem::exists(testdataDir)) {
            const std::string error = "Test Directory is missing: " + testdataDir.string();
            std::cerr << error << std::endl;
            throw std::runtime_error(error);
        }
        return testdataDir.append(fileName).string();
    }

    void checkExpected(Json::Value data, const segysdk::segyindex::SegyIndexImpl& index) {
        ASSERT_TRUE(data.isMember("Expected"));
        const auto& expected = data["Expected"];
        for (const auto& expectedOffset : expected) {
            const size_t offset = expectedOffset["Offset"].asUInt64();
            if (expectedOffset.isMember("CMP")) {
                const int cmp = expectedOffset["CMP"].asInt();
                EXPECT_EQ(offset, index.getOffsetForCMP(cmp));
            }
            if (expectedOffset.isMember("TraceNo")) {
                const int traceNo = expectedOffset["TraceNo"].asInt();
                EXPECT_EQ(offset, index.getOffsetForTraceNo(traceNo));
            }
        }
    }

    static inline const std::string dataDir2D_ = "SegyIndexUnitTestData/2D";
    std::string testFileName_;
    Json::Value json_;
};

TEST_P(Index2DGeneratingUnitTests, GenerateIndex)
{
    constexpr size_t sampleSize = 4;

    int errors = 0;
    std::vector<std::shared_ptr<IndexBuilderCollatedData>> collatedIndexDataVector = JsonUnpacker::getIndexBuilderDataFromJson(json_["IndexData"], errors);
    EXPECT_EQ(errors, 0);

    std::string testFile = "Dummy.sgy";
    std::vector segyFileNames{testFile};

    fileio::FileReaderOptions readerOptions(SDCredentials::none(), CacheSizeOption::none());
    SegyIndexImpl index(/*segyFilesVector*/ {}, segyFileNames, readerOptions);

    index.setSegy2DFlag(true);

    ASSERT_TRUE(index.ProcessIndexBuildingData(collatedIndexDataVector,
                                               segyio::VerticalDomain::Time,
                                               segyio::VerticalUOM::seconds,
                                               sampleSize));
    index.Save();
    checkExpected(json_, index);
}

INSTANTIATE_TEST_CASE_P(
    Segy2DIndexTests,
    Index2DGeneratingUnitTests,
    ::testing::Values(
        "Index2DTestData-CMP-101-Incr-1-Traces-500.json",
        "Index2DTestData-CMP-101-Incr-2-Skip-Traces-200.json",
        "Index2DTestData-CMP-23-Dif-Incr-Skip-Traces.json",
        "Index2DTestData-CMP-404-Decr-4-Traces-DifSize-Skip-Traces.json"));

} // namespace segysdk::segyindex::tests
