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

#include "../SegyIndex/SegyIndex.h"
#include "../SegyIndex/SegyIndexFactory.h"
#include "../SegyIndex/SegyIndexImpl.h"

#include "../Utilities/TokenSourceFactory.h"
#include "../tests/UtilityLibrary/TestBackdoor.h"

#include <gtest/gtest.h>

#include <filesystem>

namespace segysdk::segyindex::tests
{
using multi_file::FileNameStatus;

class IndexedFileNamesCloudTest : public testing::Test
{
protected:
    void SetUp() override
    {
        const std::string sdms_url = segysdk::utilities::Environment::instance().sdUrl();
        std::string api_key = segysdk::utilities::Environment::instance().sdApiKey();
        if (api_key.empty()) {
            api_key = "SDMS App Key goes here";
            utilities::EnvironmentVariable::setStringEnv(utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY, api_key);
        }
        const SDCredentials sdCredentials{
            sdms_url,
            api_key,
            segysdk::utilities::TokenSourceFactory::getTokenSource()};

        const fileio::FileReaderOptions readerOptions{sdCredentials, CacheSizeOption{2 * 1024 * 1024, 32, 1, 1024U * 1024U}};

        // Seismic Storage setup
        std::vector<std::string> segyFileVectorCloud = {
            "sd://opendes/sntc2/segy/mig01.sgy",
            "sd://opendes/sntc2/segy/mig02.sgy",
            "sd://opendes/sntc2/segy/mig03.sgy",
            "sd://opendes/sntc2/segy/mig04.sgy"};

        indexCloud = segysdk::api::SegyIndexFactory().CreateSegyIndex(/*segyFilesVector*/ {},
                                                                      segyFileVectorCloud,
                                                                      readerOptions);
    }

    std::shared_ptr<SegyIndexImpl> getIndex()
    {
        return TestBackdoor::getImpl(indexCloud);
    }

private:

    std::shared_ptr<SegyIndex> indexCloud;
};

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_OriginalFileNames)
{
    const std::vector<std::string> segyFileVectorOriginalNamesCloud{
        "sd://opendes/sntc2/segy/mig01.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy/mig03.sgy",
        "sd://opendes/sntc2/segy/mig04.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorOriginalNamesCloud, false);
    EXPECT_EQ(status, FileNameStatus::Match);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_OneFileMissing)
{
    const std::vector<std::string> segyFileVectorMissingCloud{
        "sd://opendes/sntc2/segy/mig01.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy/mig03.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorMissingCloud, false);
    EXPECT_EQ(status, FileNameStatus::FileCountDiffers);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_OneExtraFile)
{
    const std::vector<std::string> segyFileVectorExtrasCloud{
        "sd://opendes/sntc2/segy/mig01.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy/mig03.sgy",
        "sd://opendes/sntc2/segy/mig04.sgy",
        "sd://opendes/sntc2/segy/mig05.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorExtrasCloud, false);
    EXPECT_EQ(status, FileNameStatus::FileCountDiffers);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_NonUniqueName)
{
    const std::vector<std::string> segyFileVectorNonUniqueNameCloud{
        "sd://opendes/sntc2/segy/mig01.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig02.sgy",
        "sd://opendes/sntc2/segy/mig04.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorNonUniqueNameCloud, false);
    EXPECT_EQ(status, FileNameStatus::NoMatch);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_DuplicatedFileName)
{
    const std::vector<std::string> segyFileVectorDuplicatedNameCloud{
        "sd://opendes/sntc2/segy/mig01.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy/mig02.sgy",
        "sd://opendes/sntc2/segy/mig04.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorDuplicatedNameCloud, false);
    EXPECT_EQ(status, FileNameStatus::NamesNotUnique);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_SameFileNameDifferentPath)
{
    const std::vector<std::string> segyFileVectorNewCloud{
        "sd://opendes/sntc2/segy-reprocessed/mig01.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig02.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig03.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig04.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorNewCloud, false);
    EXPECT_EQ(status, FileNameStatus::FileNameMatch);
}

TEST_F(IndexedFileNamesCloudTest, IndexedFileNames_CloudBased_CompletelyDifferentFileNames)
{
    const std::vector<std::string> segyFileVectorNoMatchCloud{
        "sd://opendes/sntc2/segy-reprocessed/NotMig01.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig02.sgy",
        "sd://opendes/sntc2/segy-reprocessed/NotMig03.sgy",
        "sd://opendes/sntc2/segy-reprocessed/mig04.sgy"};

    const auto status = getIndex()->validateIndexedFiles(segyFileVectorNoMatchCloud, false);
    EXPECT_EQ(status, FileNameStatus::NoMatch);
}

} // namespace segysdk::segyindex::tests
