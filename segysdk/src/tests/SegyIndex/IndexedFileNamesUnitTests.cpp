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

#include "Units.h"

#include "../FileIO/FileManager.h"
#include "../SegyIndex/IndexBuilderData.h"
#include "../SegyIndex/SegyIndexImpl.h"
#include "../SegyIndex/SegyIndexMultiFileStruct.h"
#include "../SegyIndex/SegyIndex.h"
#include "../SegyIndex/SegyIndexFactory.h"

#include "../Utilities/StringUtils.h"
#include "../Utilities/TokenSourceFactory.h"
#include <gtest/gtest.h>
#include "../tests/UtilityLibrary/TestBackdoor.h"

#include <filesystem>

namespace segysdk::segyindex::tests
{
using multi_file::FileNameStatus;

class IndexedFileNamesTest : public testing::Test
{
protected:
    void SetUp() override
    {
        // Local disk setup
        segyFileVector = {
            R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
            R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
            R"(d:\Data\Segy\CloudSpin\Dummy-03.sgy)",
            R"(d:\Data\Segy\CloudSpin\Dummy-04.sgy)"};
        makeNativePaths(segyFileVector);
        indexLocal = segysdk::api::SegyIndexFactory().CreateSegyIndex(/*segyFilesVector*/ {},
                                                                      segyFileVector,
                                                                      fileio::FileReaderOptions::localOnly(),
                                                                      /*indexBaseFileNameOverride*/ "DummySegy.sgy");

    }

    std::vector<std::string> segyFileVector;

    std::shared_ptr<fileio::FileManager> fileManagerLocal;
    std::shared_ptr<SegyIndex> indexLocal;

    void
    makeNativePaths(std::vector<std::string>& inputFiles) const
    {
        const char delimiter = '\\';
        const char replace = '/';
        for (auto& fstr : inputFiles) {
            utilities::StringUtils::ReplaceAll(fstr, delimiter, replace);
        }
    }

    static Geometry3D
    getDummyGeometry()
    {
        Geometry3D geometry;
        geometry.IVectorExtent = {101, 501, 1};

        geometry.JVectorExtent = {10101, 10751, 1};

        geometry.sampleExtent = {0, 4000, Units::convertMsToSecond(4), segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds};

        geometry.dataGrid.gridCell_11.i = 101;
        geometry.dataGrid.gridCell_11.j = 10101;
        geometry.dataGrid.gridCell_11.location = Coordinate(55020.0, 650010.00);

        geometry.dataGrid.gridCell_1N.i = 101;
        geometry.dataGrid.gridCell_1N.j = 10751;
        geometry.dataGrid.gridCell_1N.location = Coordinate(61887.55, 664737.50);

        geometry.dataGrid.gridCell_N1.i = 501;
        geometry.dataGrid.gridCell_N1.j = 10101;
        geometry.dataGrid.gridCell_N1.location = Coordinate(64083.08, 645783.82);

        geometry.dataGrid.gridCell_NN.i = 501;
        geometry.dataGrid.gridCell_NN.j = 10751;
        geometry.dataGrid.gridCell_NN.location = Coordinate(70950.62, 660511.32);

        geometry.dataGrid.IVectorRotationAngle = 25.0;
        geometry.dataGrid.IVectorUnitDistance = 25.0;

        geometry.dataGrid.JVectorRotationAngle = 115;
        geometry.dataGrid.JVectorUnitDistance = 25.0;

        geometry.isInlineOrdered = true;
        geometry.isRegular = true;

        return geometry;
    }
};

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_ExactlySameNamesAsOriginal)
{
    std::vector<std::string> segyFileVectorOriginalNames{
        R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-03.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-04.sgy)"};
    makeNativePaths(segyFileVectorOriginalNames);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorOriginalNames, false);
    EXPECT_EQ(status, FileNameStatus::Match);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_OneFileMissing)
{
    std::vector<std::string> segyFileVectorMissing{
        R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-03.sgy)"};
    makeNativePaths(segyFileVectorMissing);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorMissing, false);
    EXPECT_EQ(status, FileNameStatus::FileCountDiffers);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_OneFileExtra)
{
    std::vector<std::string> segyFileVectorExtras{
        R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-03.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-04.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-05.sgy)"};
    makeNativePaths(segyFileVectorExtras);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorExtras, false);
    EXPECT_EQ(status, FileNameStatus::FileCountDiffers);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_NonUniqueFileName)
{
    std::vector<std::string> segyFileVectorNonUniqueName{
        R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-03.sgy)"};
    makeNativePaths(segyFileVectorNonUniqueName);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorNonUniqueName, false);
    EXPECT_EQ(status, FileNameStatus::NoMatch);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_DuplicatedFileName)
{
    std::vector<std::string> segyFileVectorDuplicatedName{
        R"(d:\Data\Segy\CloudSpin\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-04.sgy)"};
    makeNativePaths(segyFileVectorDuplicatedName);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorDuplicatedName, false);
    EXPECT_EQ(status, FileNameStatus::NamesNotUnique);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_SameFileNamesDifferentFolder)
{

    std::vector<std::string> segyFileVectorNew{
        R"(d:\Data\Segy\CloudSpinNew\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-03.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-04.sgy)"};
    makeNativePaths(segyFileVectorNew);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorNew, false);
    EXPECT_EQ(status, FileNameStatus::FileNameMatch);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_CompletelyDifferentFileNames)
{
    std::vector<std::string> segyFileVectorNoMatch{
        R"(d:\Data\Segy\CloudSpin\NoDummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpin\NoDummy-03.sgy)",
        R"(d:\Data\Segy\CloudSpin\Dummy-04.sgy)"};
    makeNativePaths(segyFileVectorNoMatch);

    const auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorNoMatch, false);
    EXPECT_EQ(status, FileNameStatus::NoMatch);
}

TEST_F(IndexedFileNamesTest, IndexedFileNames_LocalPaths_ReconnectTest)
{
    const auto geometry3d = getDummyGeometry();

    TestBackdoor::getImpl(indexLocal)->setGeometry3D(geometry3d);
    TestBackdoor::getImpl(indexLocal)->Save();

    std::vector<std::string> segyFileVectorNew{
        R"(d:\Data\Segy\CloudSpinNew\Dummy-01.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-02.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-03.sgy)",
        R"(d:\Data\Segy\CloudSpinNew\Dummy-04.sgy)"};
    makeNativePaths(segyFileVectorNew);

    auto status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorNew, false);
    EXPECT_EQ(status, FileNameStatus::FileNameMatch);

    TestBackdoor::getImpl(indexLocal)->reconnectSEGYFilesAfterMove(segyFileVectorNew);
    ASSERT_NO_THROW();

    status = TestBackdoor::getImpl(indexLocal)->validateIndexedFiles(segyFileVectorNew, false);
    EXPECT_EQ(status, FileNameStatus::Match);
}

} // namespace segysdk::segyindex::tests
